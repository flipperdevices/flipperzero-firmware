import { defineConfig, splitVendorChunkPlugin } from 'vite'
import vue from '@vitejs/plugin-vue'
import viteCompression from 'vite-plugin-compression';
import generateCHeaders from './vite-plugin-generate-c-header';
import chalk from 'chalk'
import { viteSingleFile } from "vite-plugin-singlefile"
import { chunkSplitPlugin } from 'vite-plugin-chunk-split';
import fs from 'fs/promises';


// https://vitejs.dev/config/
export default defineConfig({
  build: {
    rollupOptions: {
        output: {
            chunkFileNames: '[name].js',
            assetFileNames: '[name].[ext]',
            entryFileNames: '[name].js',
        }
    },
  },
  plugins: [vue(), /*viteSingleFile(),*/ splitVendorChunkPlugin({

  }), chunkSplitPlugin({
    strategy: 'default'
  }), viteCompression({
    threshold: 10,
    deleteOriginFile: true,
    filter: [],
    filename: '[path][base].gz',
    success: () => {
      generateCHeaders({
        filename: 'gameboy_server_link_camera_assets',
        prefix: 'link_camera_'
      }).generateBundle()
      console.info(`${chalk.cyan('✨ [vite-plugin-generate-c-header]:')} c header created successfully `);
    }
  }), ...(()=> {
    if(process.env.NODE_ENV == 'production') {
      return [
        // {
        //   name: 'index-html-build-replacement',
        //   apply: 'build',
        //   async transformIndexHtml() {
        //     return await fs.readFile('./index.html', 'utf8');
        //   },
        // },
        // {
        //   name: 'postbuild-commands',
        //   closeBundle: async () => {
        //     generateCHeaders({
        //       filename: 'gameboy_server_link_camera_assets',
        //       prefix: 'link_camera_'
        //     }).generateBundle()
        //     console.info(`${chalk.cyan('✨ [vite-plugin-generate-c-header]:')} c header created successfully `);
        //   }
        // }
      ];
      return [viteCompression({
        threshold: 10,
        deleteOriginFile: true,
        filter: [],
        filename: '[path][base].gz',
        // success: () => {
        //   generateCHeaders({
        //     filename: 'gameboy_server_link_camera_assets'
        //   }).generateBundle()
        //   console.info(`${chalk.cyan('✨ [vite-plugin-generate-c-header]:')} c header created successfully `);
        // }
      })]
    }
    return [];
  })()],
})
