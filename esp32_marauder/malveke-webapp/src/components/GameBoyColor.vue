<template>
  <div ref="gameboy" class="gameboy" id="GameBoy">
    <div class="screen-area">
      <div class="power">
        <div class="indicator">
          <div class="led"></div>
          <span class="arc" style="z-index: 2"></span>
          <span class="arc" style="z-index: 1"></span>
          <span class="arc" style="z-index: 0"></span>
        </div>
        POWER
      </div>
      <div class="display">
        <slot />
      </div>
      
      <!-- <canvas class="display" id="mainCanvas"></canvas> -->

      <div class="label">
        <div class="title">GAME BOY</div>
        <div class="subtitle">
          <span class="c">C</span
          ><!--
     --><span class="o1">O</span
          ><!--
     --><span class="l">L</span
          ><!--
     --><span class="o2">O</span
          ><!--
     --><span class="r">R</span>
        </div>
      </div>
    </div>

    <div class="nintendo">Nintendo</div>

    <div class="controls">
      <div class="dpad">
        <div class="up"><i class="fa fa-caret-up"></i></div>
        <div class="right"><i class="fa fa-caret-right"></i></div>
        <div class="down"><i class="fa fa-caret-down"></i></div>
        <div class="left"><i class="fa fa-caret-left"></i></div>
        <div class="middle"></div>
      </div>
      <div class="a-b">
        <div class="b" @click="onPressButton('B')">B</div>
        <div class="a" @click="onPressButton('A')">A</div>
      </div>
    </div>

    <div class="start-select">
      <div class="select" @click="onPressButton('SELECT')">SELECT</div>
      <div class="start" @click="onPressButton('START')">START</div>
    </div>

    <div class="speaker">
      <div class="dot placeholder"></div>
      <div class="dot open"></div>
      <div class="dot closed"></div>
      <div class="dot open"></div>
      <div class="dot closed"></div>
      <div class="dot open"></div>
      <div class="dot closed"></div>
      <div class="dot placeholder"></div>

      <div class="dot open"></div>
      <div class="dot closed"></div>
      <div class="dot open"></div>
      <div class="dot closed"></div>
      <div class="dot open"></div>
      <div class="dot closed"></div>
      <div class="dot open"></div>
      <div class="dot closed"></div>

      <div class="dot closed"></div>
      <div class="dot open"></div>
      <div class="dot closed"></div>
      <div class="dot open"></div>
      <div class="dot closed"></div>
      <div class="dot open"></div>
      <div class="dot closed"></div>
      <div class="dot open"></div>

      <div class="dot open"></div>
      <div class="dot closed"></div>
      <div class="dot open"></div>
      <div class="dot closed"></div>
      <div class="dot open"></div>
      <div class="dot closed"></div>
      <div class="dot open"></div>
      <div class="dot closed"></div>

      <div class="dot closed"></div>
      <div class="dot open"></div>
      <div class="dot closed"></div>
      <div class="dot open"></div>
      <div class="dot closed"></div>
      <div class="dot open"></div>
      <div class="dot closed"></div>
      <div class="dot open"></div>

      <div class="dot open"></div>
      <div class="dot closed"></div>
      <div class="dot open"></div>
      <div class="dot closed"></div>
      <div class="dot open"></div>
      <div class="dot closed"></div>
      <div class="dot open"></div>
      <div class="dot closed"></div>

      <div class="dot closed"></div>
      <div class="dot open"></div>
      <div class="dot closed"></div>
      <div class="dot open"></div>
      <div class="dot closed"></div>
      <div class="dot open"></div>
      <div class="dot closed"></div>
      <div class="dot open"></div>

      <div class="dot placeholder"></div>
      <div class="dot closed"></div>
      <div class="dot open"></div>
      <div class="dot closed"></div>
      <div class="dot open"></div>
      <div class="dot closed"></div>
      <div class="dot open"></div>
      <div class="dot placeholder"></div>
    </div>
  </div>
</template>
<script>
export default {
  
  methods: {
    onPressButton(btn) {
      this.$emit('press-button', btn)
    }
  }
};
</script>
<style lang="scss" scoped>
.gameboy {
  position:relative;
  width:350px;
  height:570px;
  border-radius:20px;
  padding:20px;
  background-color:#4f50db;
  box-shadow:0px -5px 0px #0d0e51, 0px 5px 0px #3d38b5;
  font-family:sans-serif;
  -webkit-user-select:none;
  cursor:default;
  /*
  &::before {
    content:'';
    display:block;
    position:absolute;
    top:calc(100% - 200px);
    left:20%;
    width:60%;
    height:150px;
    border-radius:20px;
    background-color:#4f50db;
    box-shadow:0px 50px 200px #aaa;
    z-index:1;
  }*/
  &::after {
    content:'';
    display:block;
    position:absolute;
    top:calc(100% - 34px);
    left:5px;
    width:calc(100% - 10px);
    height:50px;
    border-radius:50%;
    background-color:#4f50db;
    box-shadow:0px 5px 0px #3d38b5;
    border-bottom:2px solid #9998eb;
  }
  
  .screen-area {
    position:relative;
    padding:35px 50px 5px 50px;
    border-radius:15px 15px 15px 15px;
    background-color:#23252d;
    color:#67879a;
    box-shadow:0px 2px 0px black, 0px -2px 0px black, -2px 0px 0px black, 2px 0px 0px black;
    
    &::after {
      content:'';
      display:block;
      position:absolute;
      top:calc(100% - 20px);
      left:5px;
      width:calc(100% - 10px);
      height:30px;
      border-radius:50%;
      background-color:#23252d;
      box-shadow:0px 2px 0px black;
    }
    
    .power {
      position:absolute;
      display:block;
      width:50px;
      left:0px;
      top:80px;
      font-size:10px;
      letter-spacing:-0.5px;
      text-align:center;
      
      .indicator {
        line-height:14px;
        
        .led {
          position:relative;
          display:inline-block;
          background-color:#ca1a21;
          width:8px;
          height:8px;
          border-radius:50%;
          box-shadow:0px 0px 10px #ff552e;
          z-index:5;
        }
        .arc {
          position:relative;
          display:inline-block;
          width:8px;
          height:8px;
          margin-left:-2px;
          
          &::before {
            position:absolute;
            left:3px;
            content:'';
            display:inline-block;
            width:8px;
            height:8px;
            border-radius:50%;
            background-color:#67879a;
          }
          &::after {
            position:absolute;
            left:0px;
            content:'';
            display:inline-block;
            width:8px;
            height:8px;
            border-radius:50%;
            background-color:#23252d;
          }
        }
      }
    }
    
    .display {
      background-color:#929d97;
      height:190px;
      width:210px;
      border-radius:3px;
      margin-bottom:15px;
    }
    
    .label {
      position:relative;
      text-align:center;
      font-size:20px;
      z-index:5;
      
      .title {
        display:inline;
        font-weight:bold;
        font-style:italic;
      }
      .subtitle {
        display:inline;
        font-family:"Comic Sans MS";
        font-weight:bold;
        font-size:22px;
        letter-spacing:-1px;
        
        .c, .o1, .l, .o2, .r {
          display:inline-block;
        }
        .c {
          color:#aa2058;
        }
        .o1 {
          color:#605bd9;
          font-size:20px;
        }
        .l {
          color:#78b930;
          transform:rotateZ(-10deg);
        }
        .o2 {
          color:#b6b524;
          font-size:20px;
        }
        .r {
          color:#317aaf;
        }
      }
    }
  }
  
  .nintendo {
    padding:0px 5px;
    width:82px;
    color:#3436bf;
    font-weight:bold;
    text-align:center;
    margin:0 auto;
    border:2px solid #3436bf;
    border-radius:11px;
    margin-top:30px;
    text-shadow:0px -2px 1px #6b67ed;
    box-shadow:0px -2px 1px #6b67ed;
  }
  
  .controls {
    display:flex;
    justify-content:space-between;
    
    .dpad {
      position:relative;
      display:inline-block;
      width:90px;
      height:90px;
      z-index:5;
      
      > * {
        width:33%;
        height:33%;
        position:absolute;
        background-color:#565e6a;
        cursor:pointer;
        
        i {
          position:relative;
          display:block;
          margin:0 auto;
          text-align:center;
          height:100%;
          font-size:28px;
          color:#333a4a;
          text-shadow:0px -0.5px 0px #aaa;
        }
      }
      
      .up {
        top:calc(0% + 4px);
        left:33%;
        border-top:4px solid black;
        border-left:4px solid black;
        border-right:4px solid black;
        border-radius:5px 5px 0px 0px;
        
        &:active {
          background:linear-gradient(to top, #565e6a 0%, #333 100%);
        }
        
        i {
          top:-5px;
        }
      }
      .right {
        top:33%;
        left:calc(66% - 4px);
        border-top:4px solid black;
        border-bottom:4px solid black;
        border-right:4px solid black;
        border-radius:0px 5px 5px 0px;
        box-shadow:0px -2px 0px #888 inset;
        
        &:active {
          background:linear-gradient(to right, #565e6a 0%, #333 100%);
        }
        
        i {
          top:-5px;
        }
      }
      .down {
        top:calc(66% - 4px);
        left:33%;
        border-left:4px solid black;
        border-bottom:4px solid black;
        border-right:4px solid black;
        border-radius:0px 0px 5px 5px;
        box-shadow:0px -2px 0px #888 inset;
        
        &:active {
          background:linear-gradient(to bottom, #565e6a 0%, #333 100%);
        }
      }
      .left {
        top:33%;
        left:calc(0% + 4px);
        border-top:4px solid black;
        border-bottom:4px solid black;
        border-left:4px solid black;
        border-radius:5px 0px 0px 5px;
        box-shadow:0px -2px 0px #888 inset;
        
        i {
          top:-5px;
        }
        
        &:active {
          background:linear-gradient(to left, #565e6a 0%, #333 100%);
        }
      }
      .middle {
        top:33%;
        left:33%;
        z-index:-5;
        
        &::after {
          content:'';
          position:absolute;
          top:20%;
          left:20%;
          display:inline-block;
          border:1px solid #6e737a;
          background:linear-gradient(to bottom, #6d7075 0%, #6d7075 30%, #23272f 70%, #23272f 100%);
          border-radius:50%;
          height:60%;
          width:60%;
        }
      }
      
      .up-down {
        background-color:#565e6a;
        border:4px solid black;
                border-radius:5px;
        width:20px;
        height:60px;
      }
      .left-right {
        background-color:#565e6a;
        border:4px solid black;
        border-radius:5px;
        width:60px;
        height:20px;
      }
    }
    .a-b {
      position:relative;
      display:inline-block;
      width:120px;
      height:90px;
      
      .a, .b {
        position:absolute;
        display:inline-block;
        font-size:22px;
        width:40px;
        height:40px;
        line-height: 40px;
        border-radius:50%;
        background-color:#2c313e;
        border-bottom:2px solid #888;
        box-shadow:-1px 1px 5px black, 0px 0px 5px black inset;
        text-shadow:0px -1px 1px #888;
        color:#2c313e;
        text-align:center;
        -webkit-user-select:none;
        cursor:pointer;
        transition:box-shadow 0.1s ease-out, border 0.1s ease-out, line-height 0.2s ease-out;
        
        &:active {
          box-shadow:-1px 1px 1px black, 0px 0px 5px black inset;
          border-width:0px;
          line-height:45px;
        }
      }
      .a {
        top:15px;
        right:10px;
      }
      .b {
        top:35px;
        left:0%;
      }
    }
  }

  .start-select {
    width:100%;
    height:60px;
    display:flex;
    justify-content:center;
    
    .select, .start {
      display:inline-block;
      color:#6b67ed;
      text-shadow:0px -1px 0px #3436bf;
      letter-spacing:-1px;
      width:60px;
      font-size:16px;
      text-align:center;
      margin-top:60px;
      
      &::before {
        content:'';
        display:block;
        margin:0 auto;
        width:30px;
        height:10px;
        margin-bottom:5px;
        border-radius:40%;
        background:linear-gradient(to bottom, #0b0a1c 0%, #0b0a1c 30%, #62636c 70%, #62636c 100%);
        background-repeat:no-repeat;
        border:2px solid #0b0a1c;
        box-shadow:0px -2px 1px #8482e9;
        cursor:pointer;
      }
      &:active::before {
        background:linear-gradient(to bottom, #0b0a1c 0%, #0b0a1c 50%, #62636c 100%);
      }
    }
  }
  
  .speaker {
    position:absolute;
    display:flex;
    width:75px;
    height:75px;
    right:15px;
    bottom:5px;
    justify-content:space-between;
    flex-wrap:wrap;
    z-index:100;
    transform:skewY(-10deg);
    
    .dot {
      width:7px;
      height:7px;
      margin:1px;
      border-radius:50%;
    }
    .dot.placeholder {
      background-color:transparent;
    }
    .dot.open {
      background-color:#0a0717;
      box-shadow:0px 0px 2px #7c7be0 inset;
    }
    .dot.closed {
      background:linear-gradient(to bottom, #6664e5 0%, #2d3590 100%);
      box-shadow:0px 0px 2px #7c7be0 inset;
    }
  }
}
body { padding:30px; display:flex; justify-content:center; align-items:center; background: linear-gradient(to top, #aaa 0%, white 100%);}
.gameboy {
  margin:0 auto;
}
</style>
