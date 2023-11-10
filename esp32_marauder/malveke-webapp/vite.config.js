import { defineConfig } from 'vite'
import vue from '@vitejs/plugin-vue'
import viteCompression from 'vite-plugin-compression';
import { resolve } from 'path'
import generateCHeaders from './vite-plugin-generate-c-header';
import chalk from 'chalk'

// https://vitejs.dev/config/
export default defineConfig({
  plugins: [
    vue(), 
    ...(()=> {
      if(process.env.NODE_ENV == 'production') {
        return [viteCompression({
          threshold: 10,
          deleteOriginalAssets: true,
          filter: [],
          filename: '[path][base].gz',
          success: () => {
            generateCHeaders().generateBundle()
            console.info(`${chalk.cyan('✨ [vite-plugin-generate-c-header]:')} c header created successfully `);
          }
        })]
      }
      return [];
    })()
  ],
  build: {
    rollupOptions: {
      input: {
        main: resolve(__dirname, 'gameboy-camera/index.html'),
        printer: resolve(__dirname, 'gameboy-printer/index.html'),
        logo: resolve(__dirname, 'gameboy-logo/index.html'),
        telegram: resolve(__dirname, 'telegram/index.html'),
      },
      output: {
        entryFileNames: `assets/[name].js`,
        chunkFileNames: `assets/[name].js`,
        assetFileNames: `assets/[name].[ext]`
      },
    },
    generate: true,
  },
})
