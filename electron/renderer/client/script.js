// Handle cancel button
const cancelButton = document.getElementById("cancel-button");
cancelButton.addEventListener("click", () => {
  clientPicker.close();
  cpp.killClient();
});

// Display file name
clientPicker.getFileName((fileName) => {
  const fileNameContainer = document.getElementById("file-name");
  fileNameContainer.innerText = fileName;
});

// Display devices
function newDevice(deviceName, ipv4) {
  let ip;
  const sendButton = document.getElementById("send-button");
  const loadingContainer = document.getElementById("loading-container");
  const loadingGif = document.getElementById("loading-gif");
  if (loadingContainer && loadingGif) {
    loadingContainer.remove();
    loadingGif.remove();
  }
  const devices = document.getElementById("devices");
  const device = document.createElement("div");
  device.className = "device";
  device.id = ipv4;
  const phoneIcon = document.createElement("img");
  phoneIcon.src = "../../../assets/icons/mobile_icon.svg";
  phoneIcon.className = "mobile-icon";
  const phoneName = document.createElement("p");
  phoneName.className = "device-name";
  phoneName.innerText = deviceName;
  device.appendChild(phoneIcon);
  device.appendChild(phoneName);
  devices.appendChild(device);
  device.addEventListener("click", () => {
    if (!ip) {
      ip = ipv4;
      device.style.border = "solid 1px black";
    } else {
      ip = null;
      device.style.border = "solid 1px rgb(220, 220, 220)";
    }
  });
  sendButton.addEventListener("click", async () => {
    if (ip == null) {
      return;
    } else {
      await cpp.useClient("break");
      await cpp.useClient(ip);
    }
  });
}
const ips = [];
const resStatus = ["BREAK", "DONE", "START", "ERROR", "CAN'T CONNECT"];
cpp.onReceiveData((event, data) => {
  data = data.trim();
  if (!resStatus.includes(data)) {
    if (!ips.includes(data)) {
      ips.push(data);
      newDevice("Device", data);
      console.log(`New device: ${data}`);
    }
  }
  if (data == "DONE") {
    clientPicker.close();
  }
  if (data == "ERROR") {
    dialog.errorDialog("ERROR", "There was an error reading the selected file");
  }
  if (data == "BREAK") {
    dialog.errorDialog("BREAK", "Client disconnected from the server");
  }
  if (data == "CAN'T CONNECT") {
    dialog.errorDialog(
      "CAN'T CONNECT",
      "Client couldn't connect to the server"
    );
  }
});
