// import setting icon & it's container
const settingIcon = document.getElementById('setting-icon');
const sideBarSettingsButtonContainer =
    document.getElementById('sidebar-settingsButton-container');
// import send file card and it's title
const sendFileCard = document.getElementById('sendFileCard');
const sendFileTitle = document.getElementById('sendFileTitle');
// import recieve file card and it's title
const receiveFileCard = document.getElementById('receiveFileCard');
const receiveFileTitle = document.getElementById('receiveFileTitle');


// changing the setting button's icon (color) based on the hover
sideBarSettingsButtonContainer.addEventListener('mouseover', () => {
  settingIcon.src = '../../../assets/icons/setting_icon_white.svg';
})
sideBarSettingsButtonContainer.addEventListener('click', () => {
  win.settings();
})
sideBarSettingsButtonContainer.addEventListener('mouseout', () => {
  settingIcon.src = '../../../assets/icons/setting_icon_black.svg';
})

// changing the title color of the send file card
sendFileCard.addEventListener('mouseover', () => {
  sendFileTitle.style.color = 'white';
})
sendFileCard.addEventListener('click', async () => {
  const filePath = await path.filePath();
  if (filePath == undefined) {
    return;
  } else {
    await cpp.spawnServer();
    await cpp.spawnSendClient();
    await cpp.useSendClient(`send ${filePath}`);
  }
})
sendFileCard.addEventListener('mouseout', () => {
  sendFileTitle.style.color = 'black';
})

// changing the title color of the receive file card
receiveFileCard.addEventListener('mouseover', () => {
  receiveFileTitle.style.color = 'white';
})
receiveFileCard.addEventListener('click', async () => {
  const receivingPath = await path.receivingPath();
  const fullReceivingPath = receivingPath + '\\';
  if (receivingPath == undefined) {
    return;
  } else {
    await cpp.spawnServer();
    await cpp.spawnReceiveClient();
    await cpp.useReceiveClient(`receive ${fullReceivingPath}`)
    console.log('Receiving Path: ', fullReceivingPath);
  }
})
receiveFileCard.addEventListener('mouseout', () => {
  receiveFileTitle.style.color = 'black';
})