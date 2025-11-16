// import send icon & it's container
const sendIcon = document.getElementById('send-icon');
const sideBarSendButtonContainer =
    document.getElementById('sidebar-sendButton-container');

// changing the send button's icon (color) based on the hover
sideBarSendButtonContainer.addEventListener('mouseover', () => {
  sendIcon.src = '../../../assets/icons/send_icon_white.svg';
})
sideBarSendButtonContainer.addEventListener('click', () => {
  win.home();
})
sideBarSendButtonContainer.addEventListener('mouseout', () => {
  sendIcon.src = '../../../assets/icons/send_icon_black.svg';
})