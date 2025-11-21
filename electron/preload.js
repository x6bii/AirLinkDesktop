const { contextBridge, ipcRenderer } = require("electron");

// Get versions from main process
contextBridge.exposeInMainWorld("versions", {
  node: () => process.versions.node,
  chrome: () => process.versions.chrome,
  electron: () => process.versions.electron,
});

// Load windows from main process & handle window buttons
contextBridge.exposeInMainWorld("win", {
  settings: () => ipcRenderer.invoke("open-settings"),
  home: () => ipcRenderer.invoke("open-home"),
  minimize: () => ipcRenderer.invoke("window-minimize"),
  toggleMaximize: () => ipcRenderer.invoke("window-toggle-maximize"),
  close: () => ipcRenderer.invoke("window-close"),
});

// Get paths from main process
contextBridge.exposeInMainWorld("path", {
  filePath: () => ipcRenderer.invoke("file-path-picker"),
  receivingPath: () => ipcRenderer.invoke("receiving-path-picker"),
});

// Communicate with cpp process via main process
contextBridge.exposeInMainWorld("cpp", {
  spawnClient: () => ipcRenderer.invoke("spawn-cpp-client"),
  spawnServer: () => ipcRenderer.invoke("spawn-cpp-server"),
  useClient: (msg) => ipcRenderer.invoke("send-command-to-cpp-client", msg),
  useServer: (msg) => ipcRenderer.invoke("send-command-to-cpp-server", msg),
  onReceiveClientData: (data) => ipcRenderer.on("client-data", data),
  onReceiveServerData: (data) => ipcRenderer.on("server-data", data),
  killClient: () => ipcRenderer.invoke("kill-cpp-client-process"),
  killServer: () => ipcRenderer.invoke("kill-cpp-server-process"),
});

// Save local data via main process
contextBridge.exposeInMainWorld("save", {
  userName: (data) => ipcRenderer.invoke("save-username", data),
});

// Get local data via main process
contextBridge.exposeInMainWorld("get", {
  userName: () => ipcRenderer.invoke("get-username"),
  ipv4Addr: () => ipcRenderer.invoke("get-ipv4-addr"),
});

// Client picker window handler
contextBridge.exposeInMainWorld("clientPicker", {
  open: (fileName) => ipcRenderer.invoke("open-client-picker", fileName),
  close: () => ipcRenderer.invoke("close-client-picker"),
  getFileName: (callback) =>
    ipcRenderer.on("file-name", (event, fileName) => {
      callback(fileName);
    }),
});

// Handle dialogs
contextBridge.exposeInMainWorld("dialog", {
  errorDialog: (title, message) =>
    ipcRenderer.invoke("show-error", { title, message }),
});
