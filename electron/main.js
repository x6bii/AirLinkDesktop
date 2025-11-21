const { app, BrowserWindow, ipcMain, dialog } = require("electron");
const path = require("path");
const { spawn } = require("child_process");
const os = require("os");
const Store = require("electron-store");

let mainWin;
let clientPickerWin;
let isClientPickerWin = false;
let client;
let server;

// Main window creation function
const createMainWindow = () => {
  mainWin = new BrowserWindow({
    width: 900,
    height: 600,
    titleBarStyle: "hidden",
    autoHideMenuBar: true,
    icon: "../assets/icons/main_logo.png",
    webPreferences: { preload: path.join(__dirname, "preload.js") },
  });
  mainWin.loadFile("./renderer/home/index.html");
};

// Client picker window creation function
const clientPickerWindow = (fileName) => {
  clientPickerWin = new BrowserWindow({
    width: 400,
    height: 600,
    titleBarStyle: "hidden",
    autoHideMenuBar: true,
    icon: "../assets/icons/main_logo.png",
    webPreferences: { preload: path.join(__dirname, "preload.js") },
  });
  clientPickerWin.loadFile("./renderer/client/clientPicker.html");
  clientPickerWin.webContents.on("did-finish-load", () => {
    clientPickerWin.webContents.send("file-name", fileName);
  });
};

// When ready event
app.whenReady().then(() => {
  createMainWindow();
});

// Handle window buttons
ipcMain.handle("window-minimize", () => {
  mainWin.minimize();
});
ipcMain.handle("window-toggle-maximize", () => {
  if (mainWin.isMaximized()) mainWin.unmaximize();
  else mainWin.maximize();
});
ipcMain.handle("window-close", () => {
  mainWin.close();
});

// Load windows inter processes
ipcMain.handle("open-settings", () => {
  mainWin.loadFile("./renderer/settings/settings.html");
});
ipcMain.handle("open-home", () => {
  mainWin.loadFile("./renderer/home/index.html");
});

// File & receiving path picker inter procces
ipcMain.handle("file-path-picker", async () => {
  const result = await dialog.showOpenDialog({
    properties: ["openFile"],
  });
  return result.filePaths[0];
});
ipcMain.handle("receiving-path-picker", async () => {
  const result = await dialog.showOpenDialog({
    properties: ["openDirectory"],
  });
  return result.filePaths[0];
});

// Spawn client.cpp network protocol process
ipcMain.handle("spawn-cpp-client", () => {
  client = spawn("../builds/client.exe");
  // Send client.cpp output (std::cout) to preloader
  client.stdout.on("data", (data) => {
    data = data.toString();
    if (isClientPickerWin) {
      clientPickerWin.webContents.send("client-data", data);
    }
  });
  client.stdout.on("data", (data) => {
    data = data.toString();
    mainWin.webContents.send("client-data", data);
  });
});

// Spawn receive server.cpp network protocol process
ipcMain.handle("spawn-cpp-server", () => {
  server = spawn("../builds/server.exe");
  server.stdout.on("data", (data) => {
    data = data.toString();
    mainWin.webContents.send("server-data", data);
  });
});

// Send commands to client.cpp network protocol process
ipcMain.handle("send-command-to-cpp-client", (event, msg) => {
  client.stdin.write(msg + "\n");
});

// Send commands to receive server.cpp network protocol process
ipcMain.handle("send-command-to-cpp-server", (event, msg) => {
  server.stdin.write(msg + "\n");
});

// Kill client.cpp netwrok protocol process
ipcMain.handle("kill-cpp-client-process", () => {
  if (client) {
    client.kill();
  } else {
    return;
  }
});

// Kill server.cpp netwrok protocol process
ipcMain.handle("kill-cpp-server-process", () => {
  if (server) {
    server.kill();
  } else {
    return;
  }
});

// Settings local data
const settings = new Store();
ipcMain.handle("save-username", (event, data) => {
  settings.set("username", data);
});
ipcMain.handle("get-username", () => {
  if (settings.get("username") == undefined) {
    settings.set("username", os.hostname());
    return settings.get("username");
  } else {
    return settings.get("username");
  }
});

// Send ipv4 address
ipcMain.handle("get-ipv4-addr", () => {
  function getIPv4() {
    const nets = os.networkInterfaces();
    let ipv4 = null;
    for (const name of Object.keys(nets)) {
      for (const net of nets[name]) {
        if (net.family === "IPv4" && !net.internal) {
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
ipcMain.handle("open-client-picker", (event, fileName) => {
  isClientPickerWin = true;
  clientPickerWindow(fileName);
});
ipcMain.handle("close-client-picker", () => {
  isClientPickerWin = false;
  clientPickerWin.close();
});

// Handle dialogs
ipcMain.handle("show-error", (event, { title, message }) => {
  dialog.showErrorBox(title, message);
});

// App quitter
app.on("windows-all-closed", () => {
  if (process.platform !== "darwin") {
    if (client) {
      client.kill();
    }
    if (server) {
      server.kill();
    }
    app.quit();
    return;
  }
});
