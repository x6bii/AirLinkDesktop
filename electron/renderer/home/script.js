// Window buttons handler
const minimizeButton = document.getElementById("minimize-button");
const fullScreenButton = document.getElementById("full-screen-button");
const closeButton = document.getElementById("close-button");
minimizeButton.addEventListener("mouseover", () => {
  minimizeButton.src = "../../../assets/icons/minimize_button_on.png";
});
minimizeButton.addEventListener("click", () => {
  win.minimize();
});
minimizeButton.addEventListener("mouseout", () => {
  minimizeButton.src = "../../../assets/icons/minimize_button_off.png";
});
fullScreenButton.addEventListener("mouseover", () => {
  fullScreenButton.src = "../../../assets/icons/full_screen_button_on.png";
});
fullScreenButton.addEventListener("click", () => {
  win.toggleMaximize();
});
fullScreenButton.addEventListener("mouseout", () => {
  fullScreenButton.src = "../../../assets/icons/full_screen_button_off.png";
});
closeButton.addEventListener("mouseover", () => {
  closeButton.src = "../../../assets/icons/close_button_on.png";
});
closeButton.addEventListener("click", () => {
  win.close();
});
closeButton.addEventListener("mouseout", () => {
  closeButton.src = "../../../assets/icons/close_button_off.png";
});

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

// Start a query
function newQuery(type) {
  const queryContainer = document.getElementById("query-items-container");
  queryContainer.style.flexDirection = "";
  const noDataImg = document.getElementById("no-data-img");
  const noDataText = document.getElementById("no-data-text");
  if (noDataImg) noDataImg.remove();
  if (noDataText) noDataText.remove();
  const queryItem = document.createElement("div");
  queryItem.className = "query-item";
  const queryTitle = document.createElement("div");
  queryTitle.className = "query-item-title";
  const queryText = document.createElement("div");
  queryText.className = "query-item-text";
  const uploadTitle = document.createElement("h2");
  uploadTitle.className = "queryTitle";
  const uploadText = document.createElement("p");
  uploadText.className = "queryText";
  const progressBar = document.createElement("progress");
  progressBar.className = "progressBar";
  progressBar.max = "100";
  const percentage = [
    "0",
    "5",
    "10",
    "15",
    "20",
    "25",
    "30",
    "35",
    "40",
    "45",
    "50",
    "55",
    "60",
    "65",
    "70",
    "75",
    "80",
    "85",
    "90",
    "95",
    "100",
  ];

  if (type === "sending") {
    const uploadCard = document.createElement("img");
    uploadCard.src = "../../../assets/icons/upload_card.png";
    uploadCard.className = "queryCardIcon";
    uploadTitle.innerText = "Sending file...";

    cpp.onReceiveClientData((event, data) => {
      if (typeof data === "string") data = data.trim();
      if (percentage.includes(data)) {
        console.log("sending percentage: ", data);
        progressBar.value = data;
      }
      if (data.startsWith("NAME: ")) {
        uploadText.innerText = data.slice(5);
      }
      if (data === "DONE") {
        uploadTitle.innerText = "File sent";
        progressBar.value = "100";
        setTimeout(() => {
          queryItem.remove();
          if (!noDataImg) {
            const noDataImg = document.createElement("img");
            noDataImg.src = "../../../assets/icons/no_file.png";
            noDataImg.id = "no-data-img";
            noDataImg.className = "no-data-img";
            const noDataText = document.createElement("h2");
            noDataText.innerText = "No Query Items";
            noDataText.className = "no-data-text";
            noDataText.id = "no-data-text";
            queryContainer.appendChild(noDataImg);
            queryContainer.appendChild(noDataText);
            queryContainer.style.flexDirection = "column";
          }
        }, 3000);
      }
      if (data == "ERROR") {
        dialog.errorDialog(
          "ERROR",
          "There was an error reading the selected file"
        );
        setTimeout(() => {
          queryItem.remove();
          if (!noDataImg) {
            const noDataImg = document.createElement("img");
            noDataImg.src = "../../../assets/icons/no_file.png";
            noDataImg.id = "no-data-img";
            noDataImg.className = "no-data-img";
            const noDataText = document.createElement("h2");
            noDataText.innerText = "No Query Items";
            noDataText.className = "no-data-text";
            noDataText.id = "no-data-text";
            queryContainer.appendChild(noDataImg);
            queryContainer.appendChild(noDataText);
            queryContainer.style.flexDirection = "column";
          }
        }, 3000);
      }
      if (data == "BREAK") {
        dialog.errorDialog("BREAK", "Server disconnected");
        setTimeout(() => {
          queryItem.remove();
          if (!noDataImg) {
            const noDataImg = document.createElement("img");
            noDataImg.src = "../../../assets/icons/no_file.png";
            noDataImg.id = "no-data-img";
            noDataImg.className = "no-data-img";
            const noDataText = document.createElement("h2");
            noDataText.innerText = "No Query Items";
            noDataText.className = "no-data-text";
            noDataText.id = "no-data-text";
            queryContainer.appendChild(noDataImg);
            queryContainer.appendChild(noDataText);
            queryContainer.style.flexDirection = "column";
          }
        }, 3000);
      }
      if (data == "CAN'T CONNECT") {
        dialog.errorDialog(
          "CAN'T CONNECT",
          "Client couldn't connect to the server"
        );
        setTimeout(() => {
          queryItem.remove();
          if (!noDataImg) {
            const noDataImg = document.createElement("img");
            noDataImg.src = "../../../assets/icons/no_file.png";
            noDataImg.id = "no-data-img";
            noDataImg.className = "no-data-img";
            const noDataText = document.createElement("h2");
            noDataText.innerText = "No Query Items";
            noDataText.className = "no-data-text";
            noDataText.id = "no-data-text";
            queryContainer.appendChild(noDataImg);
            queryContainer.appendChild(noDataText);
            queryContainer.style.flexDirection = "column";
          }
        }, 3000);
      }
    });

    queryTitle.appendChild(uploadCard);
  } else if (type === "receiving") {
    const downloadCard = document.createElement("img");
    downloadCard.src = "../../../assets/icons/download_card.png";
    downloadCard.className = "queryCardIcon";
    uploadTitle.innerText = "Receiving file...";

    cpp.onReceiveServerData((event, data) => {
      if (typeof data === "string") data = data.trim();
      if (percentage.includes(data)) {
        console.log("receiving percentage: ", data);
        progressBar.value = data;
      }
      if (data.startsWith("NAME: ")) {
        uploadText.innerText = data.slice(5);
      }
      if (data === "DONE") {
        uploadTitle.innerText = "File received";
        progressBar.value = "100";
        setTimeout(() => {
          queryItem.remove();
          if (!noDataImg) {
            const noDataImg = document.createElement("img");
            noDataImg.src = "../../../assets/icons/no_file.png";
            noDataImg.id = "no-data-img";
            noDataImg.className = "no-data-img";
            const noDataText = document.createElement("h2");
            noDataText.innerText = "No Query Items";
            noDataText.className = "no-data-text";
            noDataText.id = "no-data-text";
            queryContainer.appendChild(noDataImg);
            queryContainer.appendChild(noDataText);
            queryContainer.style.flexDirection = "column";
          }
        }, 3000);
      }
      if (data == "ERROR") {
        dialog.errorDialog(
          "ERROR",
          "There was an error creating the receiving file"
        );
        setTimeout(() => {
          queryItem.remove();
          if (!noDataImg) {
            const noDataImg = document.createElement("img");
            noDataImg.src = "../../../assets/icons/no_file.png";
            noDataImg.id = "no-data-img";
            noDataImg.className = "no-data-img";
            const noDataText = document.createElement("h2");
            noDataText.innerText = "No Query Items";
            noDataText.className = "no-data-text";
            noDataText.id = "no-data-text";
            queryContainer.appendChild(noDataImg);
            queryContainer.appendChild(noDataText);
            queryContainer.style.flexDirection = "column";
          }
        }, 3000);
      }
      if (data == "BREAKED") {
        dialog.errorDialog("BREAK", "Client disconnected from the server");
        setTimeout(() => {
          queryItem.remove();
          if (!noDataImg) {
            const noDataImg = document.createElement("img");
            noDataImg.src = "../../../assets/icons/no_file.png";
            noDataImg.id = "no-data-img";
            noDataImg.className = "no-data-img";
            const noDataText = document.createElement("h2");
            noDataText.innerText = "No Query Items";
            noDataText.className = "no-data-text";
            noDataText.id = "no-data-text";
            queryContainer.appendChild(noDataImg);
            queryContainer.appendChild(noDataText);
            queryContainer.style.flexDirection = "column";
          }
        }, 3000);
      }
    });

    queryTitle.appendChild(downloadCard);
  }

  queryText.appendChild(uploadTitle);
  queryText.appendChild(uploadText);
  queryTitle.appendChild(queryText);
  queryItem.appendChild(queryTitle);
  queryItem.appendChild(progressBar);
  queryContainer.appendChild(queryItem);
}
cpp.onReceiveServerData((event, data) => {
  if (typeof data === "string") data = data.trim();
  if (data === "START") {
    newQuery("receiving");
  }
});
cpp.onReceiveClientData((event, data) => {
  if (typeof data === "string") data = data.trim();
  if (data === "START") {
    newQuery("sending");
  }
});
