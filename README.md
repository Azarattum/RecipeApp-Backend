# Recipe App [Back End]
Searches recipes by ingredients. Check out our [frontend UI](https://github.com/Azarattum/RecipeApp-Frontend).

## Features:
  - Host static webserver with API
  - Search available ingredients
  - Search recipe by ingredients
  - Get your recipe

### Usage:
Run in *./dist* folder:
```sh
./app
```

### Installation: 
1) Clone the repository
```sh
git clone https://github.com/Azarattum/RecipeApp.git
cd RecipeApp
```
2) Install build dependencies (example for APT):
```sh
sudo apt install libssl-dev libboost-system-dev
```
3) Make sure you have russian localizations on your system enabled. Open */etc/locale.gen* in any text editor:
```sh
sudo nano /etc/locale.gen
```
4) This line should be uncommented:
```
ru_RU.UTF-8 UTF-8
```
5) Then regenerate your locales:
```sh
sudo locale-gen
```

### Make Tasks:
| Task    | Description                             |
| ------- | --------------------------------------- |
| app     | Builds the application                  |
| scripts | Builds all scripts from *./src/scripts* |
| clean   | Deletes all building artifacts          |
