const {app, BrowserWindow, ipcMain, dialog} = require('electron');
const path = require('path');
const {spawn} = require('child_process');

let win;
let sendClient;
let receiveClient;
let server;

// Window creation function
const createWindow = () => {
  win = new BrowserWindow({
    width: 800,
    height: 600,
    titleBarStyle: 'hidden',
    autoHideMenuBar: true,
    webPreferences: {preload: path.join(__dirname, 'preload.js')}
  });
  win.loadFile('./renderer/home/index.html');
};

// When ready event
app.whenReady().then(() => {
  createWindow();
});

// Load windows inter processes
ipcMain.handle('open-settings', () => {
  win.loadFile('./renderer/settings/settings.html');
})
ipcMain.handle('open-home', () => {
  win.loadFile('./renderer/home/index.html');
})

// File & receiving path picker inter procces
ipcMain.handle('file-path-picker', async () => {
  const result = await dialog.showOpenDialog({
    properties: ['openFile'],
  });
  return result.filePaths[0];
})
ipcMain.handle('receiving-path-picker', async () => {
  const result = await dialog.showOpenDialog({
    properties: ['openDirectory'],
  })
  return result.filePaths[0];
})

// Spawn send client.cpp network protocol process
ipcMain.handle('spawn-send-cpp-client', () => {
  if (sendClient) {
    return;
  } else {
    sendClient = spawn('../builds/client.exe');
  }
})

// Spawn receive client.cpp network protocol process
ipcMain.handle('spawn-receive-cpp-client', () => {
  if (receiveClient) {
    return;
  } else {
    receiveClient = spawn('../builds/client.exe');
  }
})

// Send commands to send client.cpp network protocol process
ipcMain.handle('send-send-command-to-cpp-client', (event, msg) => {
  sendClient.stdin.write(msg + '\n');
})

// Send commands to receive client.cpp network protocol process
ipcMain.handle('send-receive-command-to-cpp-client', (event, msg) => {
  receiveClient.stdin.write(msg + '\n');
})

// Spawn server.cpp network protocol process
ipcMain.handle('spawn-server-cpp-process', () => {
  if (server) {
    return;
  } else {
    server = spawn('../builds/server.exe');
  }
})

// Kill client.cpp netwrok protocol process
ipcMain.handle('kill-client-cpp-process', () => {
  if (sendClient) {
    sendClient.kill();
  } else if (receiveClient) {
    receiveClient.kill();
  } else {
    return;
  }
})

// Kill server.cpp netwrok protocol process
ipcMain.handle('kill-server-cpp-process', () => {
  if (server) {
    server.kill();
  } else {
    return;
  }
})

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
})