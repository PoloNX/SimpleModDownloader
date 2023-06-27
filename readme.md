<div align="center">
    <h1>SimpleModDownloader</h1>
    <p>A switch homebrew which downloads mods from gamebanana</p>
</div>

<p align="center">
    <a rel="LICENSE" href="https://github.com/PoloNX/SimpleModDownloader/blob/master/LICENSE">
        <img src="https://img.shields.io/static/v1?label=license&message=GPLV3&labelColor=111111&color=0057da&style=for-the-badge&logo=data%3Aimage/png%3Bbase64%2CiVBORw0KGgoAAAANSUhEUgAAABQAAAATCAYAAACQjC21AAAACXBIWXMAAAsTAAALEwEAmpwYAAAAIGNIUk0AAHpFAACAgwAA/FcAAIDoAAB5FgAA8QEAADtfAAAcheDStWoAAAFGSURBVHjarJK9LgRhFIafWUuiEH/rJwrJClEq3IELUKgo3IrETWh0FC7BNVih0AoKBQoEydq11qMwm5yMsbPEm3yZd55zvnfO92VQKVhLak09UZeL%2BrsVZ9Qdv2tXnf1NYEndUushZFGthvemuq32FwWuq%2BeZid5DvZGpXambeYGr6qnd9dGldqaudQL3QuFWvVbbmaC6%2BprDr9WbwA4SdQW4BwaABb50CTykfjjwC%2BAx9SPAfOANYDxRCXpOnxNAM4ePA63Ul8NHR4E2QClsGgGG0jUR%2BFjglcAn8/pj4HTwUz/42FPJ68lOSDhCkR/O46XM0Qh3VcRH83jph%2BZefKUosBr8XA%2B%2BmufLAR4Dh6k/CrzWA691YOc/3Ejv6iNM3k59Xw%2B8D3gC9hN1ErjjfzSbqHVg8J8CG2XgBXgL4/9VCdD6HACaHdcHGCRMgQAAAABJRU5ErkJggg%3D%3D" alt=License>
    </a>
    <a rel="VERSION" href="https://github.com/PoloNX/AtmoPackUpdater">
        <img src="https://img.shields.io/static/v1?label=version&message=1.0.2&labelColor=111111&color=06f&style=for-the-badge" alt="Version">
    </a>
    <a rel="BUILD" href="https://github.com/PoloNX/SimpleModDownloader/actions">
        <img src="https://img.shields.io/github/actions/workflow/status/PoloNX/AtmoPackUpdater/c-cpp.yml?branch=master &labelColor=111111&color=06f&style=for-the-badge" alt=Build>
    </a>
</p>

---
  
     

- [Features](#features)
- [Screenshot](#screenshot)
- [How to build](#how-to-build)
- [Help me](#help-me)
- [Credits](#credits)

## Features

- Download mods from gamebanana directly from your switch
- Install mods directly from your switch with [SimpleModManager](https://github.com/nadrino/SimpleModManager)

## Screenshot

![](./screenshots/1.jpg)

<details>
  <summary><b>More Screenshots</b></summary>

![](./screenshots/2.jpg)
![](./screenshots/3.jpg)
![](./screenshots/4.jpg)

</details>

## How to build

### Requirements

- [devkitPro](https://devkitpro.org/wiki/Getting_Started)

### Build

```bash
sudo pacman -S switch-curl switch-zlib switch-glfw switch-mesa switch-glm switch-libarchive  
git clone --recursive https://github.com/PoloNX/SimpleModDownloader/
cd SimpleModDownloader
make
```

## Help me

If you want to help me open an issue when you encounter a bug and a pull request if you have a fix.

## Credits 

- Thanks to [Natinusala](https://github.com/natinusala) and [HamletDuFromage](https://github.com/HamletDuFromage) for [borealis](https://github.com/HamletDuFromage/borealis)
- Thanks to [nadrino](https://github.com/nadrino) for [SimpleModManager](https://github.com/nadrino/SimpleModManager)
- Thanks to [GameBanana](https://gamebanana.com/) for their API
- Thanks to [Pikatsuto](https://github.com/Pikatsuto) for the logo and SuperButterScotch for the typo
- Thanks to [NicholeMattera](https://github.com/NicholeMattera) for [Simple-INI-Parser](https://github.com/NicholeMattera/Simple-INI-Parser)

- Thanks to [JerryWn12](https://github.com/JerryWn12) for chinese translation
- Thanks to [iGlitch](https://github.com/iGlitch)for japanese translation
- Thanks to [ExFabian](https://github.com/ExFabian) for romanian translation
- Thanks to [spkats1](https://github.com/spkats1) and [tsamerman](https://github.com/tsamerman) for greek translation
- Thanks to [pixeldeadlock](https://github.com/pixeldeadlock) for brazilian portuguese translation*
- Thanks to [garobcsi](https://github.com/garobcsi) for hungarian translation


