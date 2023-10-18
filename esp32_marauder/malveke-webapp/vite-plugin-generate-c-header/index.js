import fs  from 'fs';
import path  from 'path';
import { convertToString, prepareBinary,  NUM_COLUMNS } from './stringConverter'

function getAllFiles(dirPath) {
    const files = fs.readdirSync(dirPath);
    const fileList = [];
    files.forEach((file) => {
      const filePath = path.join(dirPath, file);
      const stat = fs.statSync(filePath);
  
      if (stat.isDirectory()) {
        fileList.push(...getAllFiles(filePath)); // Recursivamente agregar archivos de subcarpetas.
      } else {
        if(filePath.match(/\.gz/g) && !filePath.match(/\.DS_Store/g)) {
          const meta = {
              size: stat.size,
              filename: path.basename(filePath),
              path: (filePath.slice(filePath.indexOf('dist/') + 5)).replace(/-/g,'_').replace(/\//g,'_').replace(/\./g,'_')
          };
          const binBinaryFileHolder = fs.readFileSync(filePath);
          const byteArray = prepareBinary(binBinaryFileHolder);
          const convertedData = convertToString(byteArray, NUM_COLUMNS, meta.path);
          fileList.push(convertedData)
        }
      }
    });
    return fileList;
  }

export default function GenerateCHeaders(options = {}) {

    const { filename = 'gameboy_server_assets.h' } = options;

    return {
      name: 'file-list-plugin',
      generateBundle(_, bundle) {
        const distPath = path.resolve(__dirname, '../dist');
        const outputFile = path.resolve(__dirname , `../../${filename}`);
        const fileList = getAllFiles(distPath).join('\n');
        fs.writeFileSync(outputFile, `#ifndef gameboy_server_assets_h\n#define gameboy_server_assets_h\n\n${fileList}\n#endif`, 'utf-8');
      },
    };
};