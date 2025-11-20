const {app, BrowserWindow, ipcMain, dialog} = require('electron');
const path = require('path');
const {spawn} = require('child_process');
const os = require('os');
const Store = require('electron-store');

let mainWin;
let clientPickerWin;
let sendClient;
let receiveClient;
let server;

// Main window creation function
const createMainWindow = () => {
  mainWin = new BrowserWindow({
    width: 900,
    height: 600,
    titleBarStyle: 'hidden',
    autoHideMenuBar: true,
    icon: '../assets/icons/main_logo.png',
    webPreferences: {preload: path.join(__dirname, 'preload.js')},
  });
  mainWin.loadFile('./renderer/home/index.html');
};

// Client picker window creation function
const clientPickerWindow = (fileName) => {
  clientPickerWin = new BrowserWindow({
    width: 400,
    height: 600,
    titleBarStyle: 'hidden',
    autoHideMenuBar: true,
    icon: '../assets/icons/main_logo.png',
    webPreferences: {preload: path.join(__dirname, 'preload.js')},
  });
  clientPickerWin.loadFile('./renderer/client/clientPicker.html');
  clientPickerWin.webContents.on('did-finish-load', () => {
    clientPickerWin.webContents.send('file-name', fileName);
  });
};

// When ready event
app.whenReady().then(() => {
  createMainWindow();
});

// Load windows inter processes
ipcMain.handle('open-settings', () => {
  mainWin.loadFile('./renderer/settings/settings.html');
});
ipcMain.handle('open-home', () => {
  mainWin.loadFile('./renderer/home/index.html');
});

// File & receiving path picker inter procces
ipcMain.handle('file-path-picker', async () => {
  const result = await dialog.showOpenDialog({
    properties: ['openFile'],
  });
  return result.filePaths[0];
});
ipcMain.handle('receiving-path-picker', async () => {
  const result = await dialog.showOpenDialog({
    properties: ['openDirectory'],
  });
  return result.filePaths[0];
});

// Spawn send client.cpp network protocol process
ipcMain.handle('spawn-send-cpp-client', () => {
  if (sendClient) {
    return;
  } else {
    sendClient = spawn('../builds/client.exe');
  }
});

// Spawn receive client.cpp network protocol process
ipcMain.handle('spawn-receive-cpp-client', () => {
  if (receiveClient) {
    return;
  } else {
    receiveClient = spawn('../builds/client.exe');
  }
});

// Send commands to send client.cpp network protocol process
ipcMain.handle('send-send-command-to-cpp-client', (event, msg) => {
  sendClient.stdin.write(msg + '\n');
});

// Send commands to receive client.cpp network protocol process
ipcMain.handle('send-receive-command-to-cpp-client', (event, msg) => {
  receiveClient.stdin.write(msg + '\n');
});

// Spawn server.cpp network protocol process
ipcMain.handle('spawn-server-cpp-process', () => {
  if (server) {
    return;
  } else {
    server = spawn('../builds/server.exe');
  }
});

// Kill client.cpp netwrok protocol process
ipcMain.handle('kill-client-cpp-process', () => {
  if (sendClient) {
    sendClient.kill();
  } else if (receiveClient) {
    receiveClient.kill();
  } else {
    return;
  }
});

// Kill server.cpp netwrok protocol process
ipcMain.handle('kill-server-cpp-process', () => {
  if (server) {
    server.kill();
  } else {
    return;
  }
});

// Settings local data
const settings = new Store();
ipcMain.handle('save-username', (event, data) => {
  settings.set('username', data);
});
ipcMain.handle('get-username', () => {
  if (settings.get('username') == undefined) {
    settings.set('username', os.hostname());
    return settings.get('username');
  } else {
    return settings.get('username');
  }
});

// Send ipv4 address
ipcMain.handle('get-ipv4-addr', () => {
  function getIPv4() {
    const nets = os.networkInterfaces();
    let ipv4 = null;
    for (const name of Object.keys(nets)) {
      for (const net of nets[name]) {
        if (net.family === 'IPv4' && !net.internal) {
          ipv4 = net.address;
          break;
        }
      }
      if (ipv4) break;
    }
    return ipv4;
  }
  return getIPv4();
});

// Handle client picker window
ipcMain.handle('open-client-picker', (event, fileName) => {
  clientPickerWindow(fileName);
});
ipcMain.handle('close-client-picker', () => {
  clientPickerWin.close();
});

// App quitter
app.on('windows-all-closed', () => {
  if (process.platform !== 'darwin') {
    if (sendClient) {
      sendClient.kill();
    } else if (receiveClient) {
      receiveClient.kil();
    } else if (server) {
      server.kill();
    } else {
      app.quit();
      return;
    }
  }
});
