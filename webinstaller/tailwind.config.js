const defaultTheme = require('tailwindcss/defaultTheme')
/** @type {import('tailwindcss').Config} */
export default {
  purge: ['./index.html', './src/**/*.{vue,js,ts,jsx,tsx}'],
  darkMode: false, // or 'media' or 'class'
  theme: {
    extend: {
      ...defaultTheme.extend,
      colors: {
        'primary': '#fe8a2c',
        'secondary': '#000000',
        'flprborder': '#76380b',
        'rfid': '#FFF493',
        'sub-1ghz': '#A5F4BF',
        'nfc': '#98CEFF',
        'ibutton': '#E1BBA6',
        'infrared': '#FF928B',
        'badusb': '#FFBEE9',
        'update': '#2ED832',
        'lightgreen': '#2fd731',
        /*
        readonly property color lightgreen: "#2ed832"
        readonly property color mediumgreen1: "#285b12"
        readonly property color mediumgreen2: "#203812"
        readonly property color darkgreen: "#0c160c"
        */
        'lightblue': '#228cff'
        /*
        readonly property color lightblue: "#228cff"
        readonly property color mediumblue: "#143c66"
        readonly property color darkblue1: "#11355c"
        readonly property color darkblue2: "#152b47"
        */
      },
      backgroundImage: {
        'flipper': "url('/flipper.svg')",
        'flipper-default': "url(/default.svg)",
        'typec': 'url(/typec.svg)',
        'close': 'url(/close.svg)',
        'minimize': 'url(/minimize.svg)'
      }
    },
    fontFamily: {
      ...defaultTheme.fontFamily,
      'born2bsporty': ['Born2bSportyV2', 'sans-serif'],
      'haxrcorp-4089': ['haxrcorp-4089', 'sans-serif']
    }
  },
  variants: {
    extend: {},
  },
  plugins: [],
}

