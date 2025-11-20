// Changing the setting button's icon (color) based on the hover
const settingIcon = document.getElementById("setting-icon");
const sideBarSettingsButtonContainer = document.getElementById(
  "sidebar-settingsButton-container"
);
sideBarSettingsButtonContainer.addEventListener("mouseover", () => {
  settingIcon.src = "../../../assets/icons/setting_icon_white.svg";
});
sideBarSettingsButtonContainer.addEventListener("click", () => {
  win.settings();
});
sideBarSettingsButtonContainer.addEventListener("mouseout", () => {
  settingIcon.src = "../../../assets/icons/setting_icon_black.svg";
});

// Changing the title color of the send file card
const sendFileCard = document.getElementById("sendFileCard");
const sendFileTitle = document.getElementById("sendFileTitle");
sendFileCard.addEventListener("mouseover", () => {
  sendFileTitle.style.color = "white";
});
sendFileCard.addEventListener("click", async () => {
  const filePath = await path.filePath();
  if (filePath == undefined) {
    return;
  } else {
    const fileName = filePath.substring(filePath.lastIndexOf("\\") + 1);
    await clientPicker.open(fileName);
    await cpp.spawnClient();
    await cpp.useClient(filePath);
  }
});
sendFileCard.addEventListener("mouseout", () => {
  sendFileTitle.style.color = "black";
});

// Changing the title color of the receive file card
const receiveFileCard = document.getElementById("receiveFileCard");
const receiveFileTitle = document.getElementById("receiveFileTitle");
receiveFileCard.addEventListener("mouseover", () => {
  receiveFileTitle.style.color = "white";
});
receiveFileCard.addEventListener("click", async () => {
  const receivingPath = await path.receivingPath();
  if (receivingPath == undefined) {
    return;
  } else {
    await cpp.spawnServer();
    await cpp.useServer(receivingPath);
  }
});
receiveFileCard.addEventListener("mouseout", () => {
  receiveFileTitle.style.color = "black";
});
