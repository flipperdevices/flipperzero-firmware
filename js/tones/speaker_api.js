({
    _acquired : false,
    _acquire : ffi("int furi_hal_speaker_acquire(int)"),
    start : ffi("void furi_hal_speaker_start(float, float)"),
    stop : ffi("void furi_hal_speaker_stop()"),
    _release : ffi("void furi_hal_speaker_release()"),
    acquire : function(timeout) {
      if (!this._acquired) {
        this._acquired = this._acquire(timeout);
      }
      return this._acquired;
    },
    acquired : function() {
      return this._acquired;
    },
    release : function() {
      if (this._acquired) {
        this._release();
        this._acquired = false;
      }
    },
    play : function(frequency, volume, duration) {
      let already_acquired = this.acquired();
      if (!already_acquired) {
        this.acquire(1000);
      };
      if (this.acquired()) {
        this.start(frequency, volume);
        delay(duration);
        this.stop();
      }
      if (!already_acquired) {
        this.release();
      }
    },
  }
)
