const { contextBridge, ipcRenderer } = require("electron");

// Get versions from main process
contextBridge.exposeInMainWorld("versions", {
  node: () => process.versions.node,
  chrome: () => process.versions.chrome,
  electron: () => process.versions.electron,
});

// Load windows from main process
contextBridge.exposeInMainWorld("win", {
  settings: () => ipcRenderer.invoke("open-settings"),
  home: () => ipcRenderer.invoke("open-home"),
});

// Get paths from main process
contextBridge.exposeInMainWorld("path", {
  filePath: () => ipcRenderer.invoke("file-path-picker"),
  receivingPath: () => ipcRenderer.invoke("receiving-path-picker"),
});

// Communicate with cpp process via main process
contextBridge.exposeInMainWorld("cpp", {
  spawnSendClient: () => ipcRenderer.invoke("spawn-send-cpp-client"),
  spawnReceiveClient: () => ipcRenderer.invoke("spawn-receive-cpp-client"),
  useSendClient: (msg) =>
    ipcRenderer.invoke("send-send-command-to-cpp-client", msg),
  useReceiveClient: (msg) =>
    ipcRenderer.invoke("send-receive-command-to-cpp-client", msg),
  spawnServer: () => ipcRenderer.invoke("spawn-server-cpp-process"),
  killClient: () => ipcRenderer.invoke("kill-client-cpp-process"),
  killServer: () => ipcRenderer.invoke("kill-server-cpp-process"),
});

// Save local data via main process
contextBridge.exposeInMainWorld("save", {
  userName: (data) => ipcRenderer.invoke("save-username", data),
});

// Get local data via main process
contextBridge.exposeInMainWorld("get", {
  userName: () => ipcRenderer.invoke("get-username"),
});
