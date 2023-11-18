import { defineConfig } from 'vite'
import vue from '@vitejs/plugin-vue'
import viteCompression from 'vite-plugin-compression';
import generateCHeaders from './vite-plugin-generate-c-header';
import chalk from 'chalk'
import { viteSingleFile } from "vite-plugin-singlefile"


// https://vitejs.dev/config/
export default defineConfig({
  plugins: [vue(), viteSingleFile(), ...(()=> {
    if(process.env.NODE_ENV == 'production') {
      return [viteCompression({
        threshold: 10,
        deleteOriginFile: true,
        filter: [],
        filename: '[path][base].gz',
        success: () => {
          generateCHeaders({
            filename: 'gameboy_server_link_camera_assets'
          }).generateBundle()
          console.info(`${chalk.cyan('✨ [vite-plugin-generate-c-header]:')} c header created successfully `);
        }
      })]
    }
    return [];
  })()],
})
