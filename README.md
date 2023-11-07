# Embedded CPP projects

## Environment setup

### Automatic format and checks with pre-commit hook

Related materials:

- Documentation: [pre-commit.com](https://pre-commit.com/)
- Blog article: [memfault - pre-commit](https://interrupt.memfault.com/blog/pre-commit)
- [Quick start](https://pre-commit.com/#quick-start)

Setup:

```bash
pip3 install  pre-commit --break-system-packages

pre-commit sample-config > .pre-commit-config.yaml

# customize pre-commit-config.yaml

# install hook
pre-commit install

# Execute hook (for staged files)
pre-commit run

# Execute hook (for all files in folder)
pre-commit run --all-files
```
