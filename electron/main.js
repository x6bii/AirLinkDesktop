const {app, BrowserWindow, ipcMain} = require('electron');
const path = require('path');

let win;

const createWindow = () => {
  win = new BrowserWindow({
    width: 800,
    height: 600,
    webPreferences: {preload: path.join(__dirname, 'preload.js')}
  });
  win.loadFile('./renderer/home/index.html');
};

app.whenReady().then(() => {
  createWindow();
});

ipcMain.handle('open-settings', () => {
  win.loadFile('./renderer/settings/settings.html');
})

ipcMain.handle('open-home', () => {
  win.loadFile('./renderer/home/index.html');
})

app.on('windows-all-closed', () => {
  if (process.platform !== 'darwin') {
    app.quit();
  }
})