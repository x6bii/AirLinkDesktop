// import send icon & it's container
const sendIcon = document.getElementById('send-icon');
const sideBarButtonsContainer =
    document.getElementById('sidebar-buttons-container');
// import setting icon & it's container
const settingIcon = document.getElementById('setting-icon');
const sideBarButtonsContainer2 =
    document.getElementById('sidebar-buttons-container2');
// import send file card and it's title
const sendFileCard = document.getElementById('sendFileCard');
const sendFileTitle = document.getElementById('sendFileTitle');
// import recieve file card and it's title
const receiveFileCard = document.getElementById('receiveFileCard');
const receiveFileTitle = document.getElementById('receiveFileTitle');


// changing the send button's icon (color) based on the hover
sideBarButtonsContainer.addEventListener('mouseover', () => {
  sendIcon.src = '../../assets/icons/send_icon_white.svg';
})
sideBarButtonsContainer.addEventListener('mouseout', () => {
  sendIcon.src = '../../assets/icons/send_icon_black.svg';
})

// changing the setting button's icon (color) based on the hover
sideBarButtonsContainer2.addEventListener('mouseover', () => {
  settingIcon.src = '../../assets/icons/setting_icon_white.svg';
})
sideBarButtonsContainer2.addEventListener('mouseout', () => {
  settingIcon.src = '../../assets/icons/setting_icon_black.svg';
})

// changing the title color of the send file card
sendFileCard.addEventListener('mouseover', () => {
  sendFileTitle.style.color = 'white';
})
sendFileCard.addEventListener('mouseout', () => {
  sendFileTitle.style.color = 'black';
})

// changing the title color of the receive file card
receiveFileCard.addEventListener('mouseover', () => {
  receiveFileTitle.style.color = 'white';
})
receiveFileCard.addEventListener('mouseout', () => {
  receiveFileTitle.style.color = 'black';
})