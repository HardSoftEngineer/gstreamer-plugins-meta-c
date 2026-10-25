#!/usr/bin/env python3
import json
import os

base = '/work/01-wr-rd/experiment'
paths = {
  'baseline': os.path.join(base,'baseline/dev/parsed.json'),
  'step-01-atomic-counters-summary': os.path.join(base,'steps/step-01-atomic-counters-summary/dev/parsed.json'),
  'step-03-deterministic-matrix+capsfilter': os.path.join(base,'steps/step-03-deterministic-matrix+capsfilter/dev/parsed.json'),
  'step-04-meta-integrity': os.path.join(base,'steps/step-04-meta-integrity/dev/parsed.json'),
  'step-05-json-summary-unified': os.path.join(base,'steps/step-05-json-summary-unified/dev/parsed.json'),
}

data = {}
for k,v in paths.items():
    if v and os.path.exists(v):
        with open(v) as f:
            data[k] = json.load(f)

# show baseline keys first, then add others
common_keys = sorted(set(data.get('baseline',{}).keys()))
# add missing
for k in data:
    for kk in data[k]:
        if kk not in common_keys:
            common_keys.append(kk)
common_keys = sorted(set(common_keys))

rows = []
for ck in common_keys:
    row = {'pipeline': ck}
    for step in ['baseline','step-01-atomic-counters-summary','step-03-deterministic-matrix+capsfilter','step-04-meta-integrity','step-05-json-summary-unified']:
        if step in data and ck in data[step]:
            d = data[step][ck]
            row[step+'_meta%'] = d['meta_present_pct']
            row[step+'_trans'] = d['transform_calls_total']
            a = d['ages_us_stats']
            row[step+'_age_avg'] = a['avg']
            row[step+'_age_p50'] = a['p50']
    rows.append(row)

with open('/work/01-wr-rd/experiment/results/comparison.md','w') as f:
    f.write('# Сравнительная таблица (baseline vs шаги)\n\n')
    f.write('Условия: 10 прогонов по 300 буферов, videotsrc -> ... -> fakesink. Docker, bind-mount.\n\n')
    f.write('| pipeline | baseline meta% | s01 meta%/trans/avg/p50 | s03(det) meta%/trans/avg/p50 | s04 meta%/trans/avg/p50 | s05 meta%/trans/avg/p50 |\n')
    f.write('|---|---|---|---|---|---|\n')
    for r in rows:
        b = str(r.get('baseline_meta%',0))
        s01 = str(r.get('step-01-atomic-counters-summary_meta%',0)) + '/' + str(r.get('step-01-atomic-counters-summary_trans',0)) + '/' + str(r.get('step-01-atomic-counters-summary_age_avg')) + '/' + str(r.get('step-01-atomic-counters-summary_age_p50'))
        s03 = str(r.get('step-03-deterministic-matrix+capsfilter_meta%',0)) + '/' + str(r.get('step-03-deterministic-matrix+capsfilter_trans',0)) + '/' + str(r.get('step-03-deterministic-matrix+capsfilter_age_avg')) + '/' + str(r.get('step-03-deterministic-matrix+capsfilter_age_p50'))
        s04 = str(r.get('step-04-meta-integrity_meta%',0)) + '/' + str(r.get('step-04-meta-integrity_trans',0)) + '/' + str(r.get('step-04-meta-integrity_age_avg')) + '/' + str(r.get('step-04-meta-integrity_age_p50'))
        s05 = str(r.get('step-05-json-summary-unified_meta%',0)) + '/' + str(r.get('step-05-json-summary-unified_trans',0)) + '/' + str(r.get('step-05-json-summary-unified_age_avg')) + '/' + str(r.get('step-05-json-summary-unified_age_p50'))
        f.write('| ' + r['pipeline'] + ' | ' + b + ' | ' + s01 + ' | ' + s03 + ' | ' + s04 + ' | ' + s05 + ' |\n')
    f.write('\n## Нюансы (+/-)\n\n### +\n- Наличие меты - объективная метрика (meta_present_pct). Качественно переносима.\n- Счётчики через SUMMARY (s01) - машиночитаемы.\n- Жёсткие capsfilter (s03) - снижают вариативность.\n- Проверка целостности (s04).\n- Детерминизм: num-buffers + >=10 прогонов.\n\n### -\n- delta meta-sys end-to-end сильно зависит от планировщика/очередей, плохо переносим в абсолютных числах между машинами.\n- videorate поведение плавающее.\n- identity drop-allocation: трансформ может не вызываться - корректно (нет копирования).\n- jpegenc!jpegdec - мета теряется при перекодировании, трансформ не спасает.\n- pad-probes в чистом gst-launch ограничены.\n\n## Рекомендации\n1. Primary метрика - % буферов с метой\n2. Sojourn time через pad-probes до/после элемента\n3. Фиксированные caps, num-buffers, >=10 прогонов\n4. Считать min/avg/p50/p90/p95/max, опираться на медиану/p90\n5. GST_DEBUG GST_META:5\n6. Разделять: нет копирования/есть копирование/перекодирование\n')

with open('/work/01-wr-rd/experiment/results/comparison.csv','w') as f:
    f.write('pipeline,baseline_meta_pct,baseline_trans,baseline_age_avg,baseline_age_p50,s01_meta_pct,s01_trans,s01_age_avg,s01_age_p50,s03_meta_pct,s03_trans,s03_age_avg,s03_age_p50,s04_meta_pct,s04_trans,s04_age_avg,s04_age_p50,s05_meta_pct,s05_trans,s05_age_avg,s05_age_p50\n')
    for r in rows:
        f.write(r['pipeline']+','+str(r.get('baseline_meta%',0))+','+str(r.get('baseline_trans',0))+','+str(r.get('baseline_age_avg'))+','+str(r.get('baseline_age_p50'))+','+str(r.get('step-01-atomic-counters-summary_meta%',0))+','+str(r.get('step-01-atomic-counters-summary_trans',0))+','+str(r.get('step-01-atomic-counters-summary_age_avg'))+','+str(r.get('step-01-atomic-counters-summary_age_p50'))+','+str(r.get('step-03-deterministic-matrix+capsfilter_meta%',0))+','+str(r.get('step-03-deterministic-matrix+capsfilter_trans',0))+','+str(r.get('step-03-deterministic-matrix+capsfilter_age_avg'))+','+str(r.get('step-03-deterministic-matrix+capsfilter_age_p50'))+','+str(r.get('step-04-meta-integrity_meta%',0))+','+str(r.get('step-04-meta-integrity_trans',0))+','+str(r.get('step-04-meta-integrity_age_avg'))+','+str(r.get('step-04-meta-integrity_age_p50'))+','+str(r.get('step-05-json-summary-unified_meta%',0))+','+str(r.get('step-05-json-summary-unified_trans',0))+','+str(r.get('step-05-json-summary-unified_age_avg'))+','+str(r.get('step-05-json-summary-unified_age_p50'))+'\n')

print('done')
