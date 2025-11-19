// Changing the send button's icon (color) based on the hover
const sendIcon = document.getElementById("send-icon");
const sideBarSendButtonContainer = document.getElementById(
  "sidebar-sendButton-container"
);
sideBarSendButtonContainer.addEventListener("mouseover", () => {
  sendIcon.src = "../../../assets/icons/send_icon_white.svg";
});
sideBarSendButtonContainer.addEventListener("click", () => {
  win.home();
});
sideBarSendButtonContainer.addEventListener("mouseout", () => {
  sendIcon.src = "../../../assets/icons/send_icon_black.svg";
});

// Load the username from main process
const profileTextContainer = document.getElementById("profile-text-container");
async function loadName() {
  const profileUsername = document.createElement("p");
  profileUsername.className = "profile-username";
  profileUsername.id = "profile-username";
  profileTextContainer.appendChild(profileUsername);
  const name = await get.userName();
  profileUsername.innerText = name;
}
loadName();

// Load ipv4 address
const ipText = document.getElementById("ip");
async function loadIpv4() {
  const ip = await get.ipv4Addr();
  if (ip == null) {
    return;
  } else {
    ipText.innerText = ip;
  }
}
loadIpv4();

// Handle edit button in settings/profile
const buttonsContainer = document.getElementById("profile-buttons-container");

function createEditButton() {
  const editButton = document.createElement("button");
  editButton.innerText = "Edit";
  editButton.className = "edit-button";
  editButton.id = "edit-button";
  buttonsContainer.appendChild(editButton);
  editButton.addEventListener("click", () => {
    const cancelButton = document.createElement("button");
    const saveButton = document.createElement("button");
    saveButton.innerText = "Save";
    saveButton.className = "edit-button";
    cancelButton.innerText = "Cancel";
    cancelButton.className = "edit-button";
    buttonsContainer.appendChild(saveButton);
    buttonsContainer.appendChild(cancelButton);
    editButton.remove();
    const profileUsername = document.getElementById("profile-username");
    profileUsername.remove();
    const profileUsernameInput = document.createElement("input");
    profileUsernameInput.value = profileUsername.innerText;
    profileUsernameInput.className = "profile-username-input";
    profileTextContainer.appendChild(profileUsernameInput);
    cancelButton.addEventListener("click", () => {
      profileUsernameInput.remove();
      loadName();
      createEditButton();
      cancelButton.remove();
      saveButton.remove();
    });
    saveButton.addEventListener("click", () => {
      if (profileUsernameInput.value.length < 1) {
        profileUsernameInput.className = "profile-username-input-error";
        return;
      } else {
        save.userName(profileUsernameInput.value);
      }
      profileUsernameInput.remove();
      loadName();
      createEditButton();
      cancelButton.remove();
      saveButton.remove();
    });
  });
}
createEditButton();
