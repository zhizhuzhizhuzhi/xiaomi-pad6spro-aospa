#!/usr/bin/env python3
# SPDX-License-Identifier: Apache-2.0
"""Install hash-pinned prepared inputs or verify them; no implicit downloads."""
import argparse,hashlib,json,pathlib,shutil
kit=pathlib.Path(__file__).resolve().parents[1]
p=argparse.ArgumentParser(description=__doc__);p.add_argument('tree',type=pathlib.Path);p.add_argument('--source',type=pathlib.Path,action='append',default=[]);p.add_argument('--verify',action='store_true');a=p.parse_args();tree=a.tree.resolve();rows=json.loads((kit/'dependencies.json').read_text())['files']
def sha(path):
 with path.open('rb') as f:return hashlib.file_digest(f,'sha256').hexdigest()
missing=[];copied=0
for r in rows:
 rel=pathlib.PurePosixPath(r['destination'])
 if rel.is_absolute() or '..' in rel.parts:raise SystemExit('Invalid destination')
 dest=tree/rel
 if dest.is_file() and sha(dest)==r['sha256']:continue
 if dest.exists():raise SystemExit('Different input already exists: '+str(rel))
 candidate=None
 if not a.verify:
  for source in a.source:
   for name in [r['destination']]+r.get('sources',[]):
    src=source/name
    if src.is_file() and sha(src)==r['sha256']:candidate=src;break
   if candidate:break
 if candidate:
  dest.parent.mkdir(parents=True,exist_ok=True);shutil.copyfile(candidate,dest);dest.chmod(r.get('mode',0o644));copied+=1
 else:missing.append(str(rel))
if missing:raise SystemExit('Missing or mismatched prepared inputs (%d):\n%s\nUse matching stock extraction and documented recipes; arbitrary replacements are rejected.'%(len(missing),'\n'.join(missing)))
print('PASS: %d hash-pinned inputs verified; %d copied'%(len(rows),copied))
