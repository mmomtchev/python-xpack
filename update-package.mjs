import { promises as fs } from 'node:fs';
import * as path from 'node:path';
import { fileURLToPath } from 'node:url';
import * as crypto from 'node:crypto';

const dir = path.dirname(fileURLToPath(import.meta.url));
const pkgJson = JSON.parse(await fs.readFile(path.resolve(dir, 'package.json'), 'utf-8'));
const files = await fs.readdir(dir);
const re = new RegExp(`${pkgJson.name}-([0-9]+\\.[0-9]+\\.[0-9]+-[0-9]+)-([\\w\\-]+)-([\\w\\-]+)`);
let version;
for (const f of files) {
  const r = re.exec(f);
  if (r === null) continue;

  if (!version)
    version = r[1];
  if (version != r[1])
    throw new Error(`Versions do not match, ${version} != ${r[1]}`);

  const data = await fs.readFile(path.resolve(dir, f));
  const hash = crypto.createHash('sha256');
  hash.update(data);
  const sig = hash.digest('hex');
  const platform = r[2].toLowerCase();
  const arch = r[3].toLowerCase();
  console.log(f.padEnd(40), `${platform}-${arch}`.padEnd(20), sig);
  if (!pkgJson.xpack.binaries)
    pkgJson.xpack.binaries = { platforms: {} };
  if (!pkgJson.xpack.binaries.platforms)
    pkgJson.xpack.binaries.platforms = {};
  pkgJson.xpack.binaries.platforms[`${platform}-${arch}`] = {
    fileName: f,
    sha256: sig
  };
}

if (!version) {
  throw new Error('No binaries found');
}

pkgJson.version = version;
pkgJson.xpack.binaries.baseUrl = pkgJson.xpack.binaries.rootUrl + '/v' + version

const output = JSON.stringify(pkgJson, undefined, '  ');
console.log(output);
await fs.writeFile(path.resolve(dir, 'package.json'), output);
