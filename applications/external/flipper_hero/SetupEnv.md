

# Flipper Development Setup with UFBT

## Quick Start

### Prerequisites
- `ufbt` tool
- Firmware version of your Flipper device

### Installation
Install `ufbt` using pip:
```bash
> pip install ufbt
```

### Configuration

**Official Firmware:**
Set `ufbt` to target a specific firmware version, e.g., 0.99.1:
```bash
> ufbt update --branch=0.99.1
```

**Unleashed Firmware:**
Set `ufbt` for the unleashed firmware, specifying the custom repository URL:
```bash
> ufbt update --index-url=https://up.unleashedflip.com/directory.json
```

### Build Your App
Build your application:
```bash
> ufbt build
```


## Note
Adjust commands to match your specific firmware version or development needs. This streamlined guide ensures your setup process is focused and efficient.

## Special thanks

Huge thanks to [u/theblackhole08](https://www.reddit.com/user/theblackhole08/) for the guidance on setting up my environment with ufbt for my Flipper hero project. Your tips were incredibly helpful, and now everything is running smoothly without any API mismatch errors. For anyone else struggling with this, [check out the thread here](https://www.reddit.com/r/flipperzero/comments/1blydyo/comment/kw89w5a/?utm_source=share&utm_medium=web3x&utm_name=web3xcss&utm_term=1&utm_content=share_button).
