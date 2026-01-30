# Clone the repository
```
git clone https://github.com/<YOUR_USERNAME>/miryoku_qmk.git
cd miryoku_qmk
```

# Sync the "Brain"
```make git-submodule```

# Build the Firmware
```
# Syntax: make <keyboard>:<keymap>:uf2 <options>
make crkbd:manna-harbour_miryoku:uf2 CONVERT_TO=rp2040_ce
```
