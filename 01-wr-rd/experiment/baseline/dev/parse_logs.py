#!/usr/bin/env python3
import json
import os
import re
from glob import glob

RAW_DIR = os.path.join(os.path.dirname(__file__), 'raw')
OUT_JSON = os.path.join(os.path.dirname(__file__), 'parsed.json')
OUT_CSV = os.path.join(os.path.dirname(__file__), 'parsed.csv')
OUT_MD = os.path.join(os.path.dirname(__file__), 'summary.md')

def parse_file(path):
    with open(path, errors='replace') as f:
        content = f.read()
    # counts
    transform_calls = content.count('gst_my_meta_transform')
    writer_lines = re.findall(r'Writer:.*\n', content)
    reader_lines = re.findall(r'Reader:.*\n', content)
    no_meta = content.count('Reader: no meta')
    unsupported = content.count('unsupported meta version')
    # parse numbers
    ages = []
    for m in re.findall(r'delta meta-sys=(\d+)', content):
        ages.append(int(m))
    frame_ids_reader = [int(x) for x in re.findall(r'Reader: frame=(\d+)', content)]
    frame_ids_writer = [int(x) for x in re.findall(r'Writer: frame=(\d+)', content)]
    return {
        'transform_calls': transform_calls,
        'writer_count': len(writer_lines),
        'reader_count': len(reader_lines),
        'reader_no_meta': no_meta,
        'unsupported': unsupported,
        'ages_us': ages,
        'frame_ids_reader': frame_ids_reader,
        'frame_ids_writer': frame_ids_writer,
    }

def stats(lst):
    if not lst:
        return {'count':0,'min':None,'avg':None,'p50':None,'p90':None,'p95':None,'max':None}
    s = sorted(lst)
    n = len(s)
    def pct(p):
        k = (n-1)*p/100.0
        f = int(k)
        c = f+1
        if c>=n: return s[f]
        return int(s[f] + (s[c]-s[f])*(k-f))
    avg = sum(s)/n
    return {'count':n,'min':s[0],'avg':round(avg,2),'p50':pct(50),'p90':pct(90),'p95':pct(95),'max':s[-1]}

def main():
    results = {}
    for logfile in glob(os.path.join(RAW_DIR, '*.log')):
        name = os.path.basename(logfile).replace('.log','')
        # group by pipeline name
        base = re.sub(r'_run\d+$', '', name)
        if base not in results:
            results[base] = []
        results[base].append(parse_file(logfile))
    
    summary = {}
    for base, runs in results.items():
        all_ages = []
        total_writer = 0
        total_reader = 0
        total_no_meta = 0
        total_transform = 0
        for r in runs:
            all_ages.extend(r['ages_us'])
            total_writer += r['writer_count']
            total_reader += r['reader_count']
            total_no_meta += r['reader_no_meta']
            total_transform += r['transform_calls']
        frames_with_meta = max(0, total_reader - total_no_meta)
        meta_present_pct = round((frames_with_meta / total_reader * 100.0), 2) if total_reader>0 else 0.0
        summary[base] = {
            'runs': len(runs),
            'writer_lines_total': total_writer,
            'reader_lines_total': total_reader,
            'reader_no_meta_total': total_no_meta,
            'frames_with_meta_total': frames_with_meta,
            'meta_present_pct': meta_present_pct,
            'transform_calls_total': total_transform,
            'ages_us_stats': stats(all_ages),
        }
    
    with open(OUT_JSON,'w') as f:
        json.dump(summary,f,indent=2)
    
    # CSV
    with open(OUT_CSV,'w') as f:
        f.write('pipeline,runs,meta_present_pct,frames_with_meta_total,reader_lines_total,reader_no_meta_total,transform_calls_total,ages_count,ages_avg,ages_p50,ages_p90,ages_p95,ages_min,ages_max\n')
        for k in sorted(summary.keys()):
            v = summary[k]
            a = v['ages_us_stats']
            f.write(f"{k},{v['runs']},{v['meta_present_pct']},{v['frames_with_meta_total']},{v['reader_lines_total']},{v['reader_no_meta_total']},{v['transform_calls_total']},{a['count']},{a['avg']},{a['p50']},{a['p90']},{a['p95']},{a['min']},{a['max']}\n")
    
    # MD
    with open(OUT_MD,'w') as f:
        f.write('# Baseline summary (original code)\n\n')
        f.write(f'- Runs per pipeline: 10 (NUM_BUFFERS=300)\n')
        f.write(f'- GST_DEBUG: GST_META:5,mymetawriter:4,mymetareader:4\n\n')
        f.write('| pipeline | runs | meta_present_pct | frames_with_meta/reader_total | no_meta | transform_calls | ages_avg_us | ages_p50 | ages_p90 | ages_p95 | ages_min-max |\n')
        f.write('|---|---|---|---|---|---|---|---|---|---|---|\n')
        for k in sorted(summary.keys()):
            v = summary[k]
            a = v['ages_us_stats']
            f.write(f"| {k} | {v['runs']} | {v['meta_present_pct']} | {v['frames_with_meta_total']}/{v['reader_lines_total']} | {v['reader_no_meta_total']} | {v['transform_calls_total']} | {a['avg']} | {a['p50']} | {a['p90']} | {a['p95']} | {a['min']}-{a['max']} |\n")
        f.write('\n## Notes\n')
        f.write('- meta_age_mono_us = g_get_monotonic_time() - meta->timestamp (end-to-end from writer attach to reader)\n')
        f.write('- transform_calls counted by string "gst_my_meta_transform" in logs (GST_META debug)\n')
        f.write('- queues increase age significantly; identity drop-allocation often avoids transform; videorate behavior varies; jpegenc!jpegdec loses meta\n')
    
    print('done')

if __name__=='__main__':
    main()
