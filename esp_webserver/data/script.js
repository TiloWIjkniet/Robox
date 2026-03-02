// Menu row
const btnInstellingen = document.getElementById("btn_Instellingen");
const btnGegevens = document.getElementById("btn_Gegevens");
const btnOverige = document.getElementById("btn_Overige");
const buttons = [btnInstellingen, btnGegevens, btnOverige];

const pages = 
{
  btn_Instellingen: document.getElementById("pageInstellingen"),
  btn_Gegevens: document.getElementById("pageGegevens"),
  btn_Overige: document.getElementById("pageOverige")
};

btnGegevens.classList.add("active");
buttons.forEach(button => 
{
    button.addEventListener("click", () => 
    {
        Object.values(pages).forEach(page => page.style.display = "none");
        pages[button.id].style.display = "block";

        buttons.forEach(btn => btn.classList.remove("active"));
        button.classList.add("active");
  });
});


// --- Data: tijd per kamer, gehaald?, fouten, totaal, moeilijkheid, max kamers ---

let Recordings =
[
    [[2,3,4,5,6],false,2,60,2,5],
    [[2,3,4,5,6],false,2,60,2,5]
];
let currentIndex = 0;

// --- Canvas setup ---
const canvas = document.createElement('canvas');
canvas.id = "myChart";
canvas.style.width = "100%";
canvas.style.height = "95%";
document.getElementById("chartContainer").appendChild(canvas);
const ctx = canvas.getContext('2d');

function drawChart() 
{
    if (Recordings.length <= currentIndex) return;
    const rect = canvas.getBoundingClientRect();
    
    // Zet de canvas pixels op deze grootte
    canvas.width = rect.width * window.devicePixelRatio;
    canvas.height = rect.height * window.devicePixelRatio;
    
    ctx.scale(window.devicePixelRatio, window.devicePixelRatio);

    const width = rect.width;
    const height = rect.height;

    ctx.clearRect(0, 0, width, height);


    const values = Recordings[currentIndex][0];
    const labels = values.map((_, i) => `${i+1}`); // automatisch labels
    const yMax = Math.max(...values) + 2;
    const barWidth = width / values.length * 0.6;
    const spacing = width / values.length * 0.4;





    // Y-as lijnen en nummers
    ctx.strokeStyle = '#b2aca2';
    ctx.fillStyle = '#b2aca2';
    ctx.font = '17px Arial';
    ctx.textAlign = 'right';
    ctx.textBaseline = 'middle';


        // Y-as titel
    ctx.save();
    ctx.translate(15, height / 2);
    ctx.rotate(-Math.PI / 2);
    ctx.textAlign = 'center';
    ctx.fillText('Tijd (min)', 0, 0);
    ctx.restore();

    for (let i = 0; i <= yMax; i++) {
        const y = height - (i / yMax) * (height - 40) - 20;
        ctx.beginPath();
        ctx.moveTo(40, y);
        ctx.lineTo(width, y);
        ctx.stroke();
        ctx.fillText(i, 35, y);
    }

    // Blokken tekenen
    values.forEach((val, i) => 
    {
        const barHeight = (val / yMax) * (height - 40);
        const x = i * (barWidth + spacing) + spacing/2 + 40;
        const y = height - barHeight - 20;

        ctx.fillStyle = 'rgba(43, 42, 51,0.8)';
        ctx.strokeStyle = 'rgba(98, 101, 112,1)';
        ctx.lineWidth = 3;

        ctx.fillRect(x, y, barWidth, barHeight);
        ctx.strokeRect(x, y, barWidth, barHeight);

            ctx.fillStyle = '#b2aca2';
        ctx.textAlign = 'center';
        ctx.fillText("Kamer "+ labels[i], x + barWidth/2, height - 11);
        ctx.fillText(val + " min",  x + barWidth/2, y - 8);
    });
}

// --- Update stats ---
function updateStats() {
    if(Recordings.length <= currentIndex) return;
    const data = Recordings[currentIndex];
    const totaal = data[0].reduce((a,b)=>a+b,0);
    const snelst = Math.min(...data[0]);
    const langzaamst = Math.max(...data[0]);
    const gemiddeld = (totaal / data[0].length).toFixed(1);

    document.getElementById("totaleTijd").textContent = formatTime(totaal);
    document.getElementById("snelsteTijd").textContent = formatTime(snelst);
    document.getElementById("langsteTijd").textContent = formatTime(langzaamst);
    document.getElementById("gemiddeldeTijd").textContent = formatTime(gemiddeld);
    document.getElementById("tijdOver").textContent = formatTime(data[3]-totaal);

    document.getElementById("geslaagd").textContent = data[1]?"Gehaald":"Niet gehaalt";
    document.getElementById("aantalFouten").textContent = data[2];
    document.getElementById("moeilijkheid").textContent = data[4]+"/5";
    document.getElementById("bomTijd").textContent = formatTime(data[3]);
    document.getElementById("kamners").textContent = `${data[0].length}/${data[5]}`;
}

// --- Format time ---
function formatTime(minutenFloat) {
    const totalSeconds = Math.round(minutenFloat*60);
    const min = Math.floor(totalSeconds/60);
    const sec = totalSeconds%60;
    let buff = `${min} min`;
    if(sec>0) buff+=` ${sec} sec`;
    return buff;
}

// --- Knoppen ---
const btnContainer = document.getElementById("recordingButtons");
function drawRecordingsButtons()
{
    console.log(Recordings);
    Recordings.forEach((_, idx) => 
    {
        const btn = document.createElement('button');
        btn.textContent = idx=== 0 ? 'Afgelopen run' : `${idx} Runs geleden`;
        const indicator = document.createElement('span');
        indicator.textContent = Recordings[idx][1]?"✓":"✗";
        indicator.style.color = Recordings[idx][1]?"green":"red";
        indicator.style.marginLeft="5px";
        btn.appendChild(indicator);

        btn.addEventListener('click', ()=>
        {
            currentIndex = idx;
            drawChart();
            updateStats();

            btnContainer.querySelectorAll("button").forEach(b=>b.classList.remove("active"));
            btn.classList.add("active");
        });

        if(idx===0) btn.classList.add("active");
        btnContainer.appendChild(btn);
    });
}


//Settings

const listWrapper = document.querySelector(".kamers-list-wrapper"); // parent container
let counter = 0;
let draggedItem = null;

let index = 0;
const emtyKamerListPos = [0,0];

// data naar die naar esp moet
let kamerList =[]; // centrale lijst van knoppen
let uploadedImageData = null; // hier slaan we de image op

let globalSettings = {};

function addItem() 
{

    if(kamerList.length >= 20) 
    {
        alert("Je kan niet meer dan 20 kamers hebben");
        return;
    }
    const btn = document.createElement("button");

    btn.textContent = "Kamer " + (listWrapper.querySelectorAll("button").length + 1);

    let KSD = 
    {
        "naam-kamer": "Kamer " + (listWrapper.querySelectorAll("button").length + 1),
        "becon-ip": "",
        "antwoord": "",
        "open-compartment": 0,
        "special-acties": 0
    };
    kamerList.push([btn, emtyKamerListPos,KSD]);
    
    counter++;

    index = kamerList.findIndex(k => k[0] === btn);
    kamerList.forEach(k => k[0].classList.remove("active"));
    btn.classList.add("active");

        const delBtn = document.createElement("span");
    delBtn.classList.add("delete-icon"); 
    delBtn.textContent = " 🗑️";
    delBtn.style.cursor = "pointer";

    delBtn.onclick = (e) => 
    {
        e.stopPropagation();
        if(kamerList.length <= 3)
        {
            alert("Je kunt niet minder dan 3 kamers hebben.");
            return;
        } 

        const ind = kamerList.findIndex(k => k[0] === btn);
        // Confirm popup bij verwijderen


        if (ind == -1) return;
        
        const wasActive = delBtn.parentElement.classList.contains("active");
        delBtn.parentElement.remove();

        kamerList.splice(ind, 1); 
        isDirty = true;

        // Verwijder uit DOM



        // Verwijder uit kamerList
 

        // Activeer eerste knop als nodig
        if (wasActive && kamerList.length > 0) {
            kamerList[0][0].classList.add("active");
        }

        updateSettings();
    };

    btn.appendChild(delBtn);

    // Click selectie
    btn.addEventListener("click", () => {
        index = kamerList.findIndex(k => k[0] === btn);
        kamerList.forEach(k => k[0].classList.remove("active"));
        btn.classList.add("active");
        updateSettings();
         isDirty = true;
        

    });

    // Drag & Drop
    btn.draggable = true;
    btn.addEventListener("dragstart", () => draggedItem = btn);
    btn.addEventListener("dragover", e => e.preventDefault());
    btn.addEventListener("drop", () => {
        if (draggedItem && draggedItem !== btn) {
            const draggedIndex = kamerList.findIndex(k => k[0] === draggedItem);
            const dropIndex = kamerList.findIndex(k => k[0] === btn);

            // Update DOM
            if (draggedIndex < dropIndex) {
                btn.after(draggedItem);
            } else {
                btn.before(draggedItem);
            }
             isDirty = true;
            // Update kamerList volgorde
            const [moved] = kamerList.splice(draggedIndex, 1);
            kamerList.splice(dropIndex, 0, moved);


        }
    });

    // Voeg knop toe aan DOM
    listWrapper.appendChild(btn);

    // Activeer eerste knop automatisch als dit de enige is
    if (kamerList.length === 1) {
        btn.classList.add("active");
    }

    // Update teksten
    updateSettings();
}

function initKamerList() 
{

    listWrapper.innerHTML = "";
    let kmaerListLength =  kamerList.length;
    
    if (kamerList.length != 0)
    {
    kamerList.forEach((kamer, i) => 
    {
        const btn = document.createElement("button");
        kamerList[i][0] = btn;
        if(kamerList[i][1] != null)kamerList[i][1] = [0,0];
        btn.classList.add("kamer-item");
        btn.textContent = kamer[2]["naam-kamer"] || "Kamer " + (i + 1);

        // Voeg delete knop toe


    index = kamerList.findIndex(k => k[0] === btn);


   
        const delBtn = document.createElement("span");
    delBtn.classList.add("delete-icon"); 
    delBtn.textContent = " 🗑️";
    delBtn.style.cursor = "pointer";

    delBtn.onclick = (e) => 
    {
        e.stopPropagation();
        if(kamerList.length <= 3)
        {
            alert("Je kunt niet minder dan 3 kamers hebben.");
            return;
        } 

        const ind = kamerList.findIndex(k => k[0] === btn);
        // Confirm popup bij verwijderen


        if (ind == -1) return;
        
        const wasActive = delBtn.parentElement.classList.contains("active");
        delBtn.parentElement.remove();

        kamerList.splice(ind, 1); 
        isDirty = true;

        // Verwijder uit DOM



        // Verwijder uit kamerList
 

        // Activeer eerste knop als nodig
        if (wasActive && kamerList.length > 0) {
            kamerList[0][0].classList.add("active");
        }

        updateSettings();
    };



    btn.appendChild(delBtn);

    // Click selectie
    btn.addEventListener("click", () => {
        index = kamerList.findIndex(k => k[0] === btn);
        kamerList.forEach(k => k[0].classList.remove("active"));
        btn.classList.add("active");
        updateSettings();       

    });

        // Voeg button toe aan kamerList[i] zodat we de referentie hebben
        kamer[0] = btn;

    btn.draggable = true;
    btn.addEventListener("dragstart", () => draggedItem = btn);
    btn.addEventListener("dragover", e => e.preventDefault());
    btn.addEventListener("drop", () => {
        if (draggedItem && draggedItem !== btn) {
            const draggedIndex = kamerList.findIndex(k => k[0] === draggedItem);
            const dropIndex = kamerList.findIndex(k => k[0] === btn);

            if (draggedIndex < dropIndex) {
                btn.after(draggedItem);
            } else {
                btn.before(draggedItem);
            }

            // Update kamerList volgorde
            const [moved] = kamerList.splice(draggedIndex, 1);
            kamerList.splice(dropIndex, 0, moved);
        }
        kamerList[kamerList.length -1][0].add("active");
    });

    // Voeg knop toe aan DOM
    listWrapper.appendChild(btn);
    });

    // Activeer de eerste kamer en laad zijn instellingen
    index = 0;
    if (kamerList.length > 0) {
        kamerList[0][0].classList.add("active");
        loadSettings(kamerSettingsDiv1, kamerList[0][2]);
        generateGridBtn(index);
    }
    }      
    if(kmaerListLength < 3)
    {
        for(let i=0; i < 3 - kmaerListLength; i++){console.log(i); addItem();}
    }
    
   console.log("Geïmporteerde data:", kamerList);
}

function updateSettings() {
  
    if (kamerList.length > index && kamerList[index][2]) 
    {
        loadSettings(kamerSettingsDiv1, kamerList[index][2]);
    }
    generateGridBtn(index);
}



const dropZone = document.getElementById("dropZone");
const imageInput = document.getElementById("imageInput");
const imagePreview = document.getElementById("imagePreview");
const placeholder = document.getElementById("placeholder");
const ratioWarning = document.getElementById("ratioWarning");

// 2:1 verhouding
const RATIO_2_1 = 2 / 1; // breedte / hoogte

// Klik op container opent file picker
dropZone.addEventListener("click", () => {
    if(uploadedImageData == null){imageInput.click(); isDirty = true;}
});

imageInput.addEventListener("click", () => {
    imageInput.click();
     isDirty = true;
});

// File input change
imageInput.addEventListener("change", () => {
    if (imageInput.files.length) {handleFile(imageInput.files[0]); isDirty = true;}
});

// Drag & Drop events
dropZone.addEventListener("dragover", (e) => {
    e.preventDefault();
    dropZone.style.borderColor = "#333";
    
});

dropZone.addEventListener("dragleave", (e) => {
    e.preventDefault();
    dropZone.style.borderColor = "#aaa";
});

dropZone.addEventListener("drop", (e) => {
    e.preventDefault();
    dropZone.style.borderColor = "#aaa";
    if (e.dataTransfer.files.length){ handleFile(e.dataTransfer.files[0]); isDirty = true;}
});

// Functie om bestand te tonen en 2:1 te checken

let myFile = null;

function handleFile(file) 
{
    if (!file.type.startsWith("image/")) return;

    myFile = file;
    const objectURL = URL.createObjectURL(file);
    const img = new Image();

    img.onload = function() 
    {
        // ===== 1️⃣ Ratio check =====
        const ratio = img.naturalWidth / img.naturalHeight;

        if (Math.abs(ratio - RATIO_2_1) / RATIO_2_1 > 0.05) 
        {
            ratioWarning.style.display = "block";
            uploadedImageData = null;

            imagePreview.src = "";
            imagePreview.style.display = "none";
            placeholder.style.display = "block";
            imageInput.value = "";

            return;
        }

        ratioWarning.style.display = "none";

        // ===== 2️⃣ Canvas maken =====
        const canvas = document.createElement("canvas");
        canvas.width = img.naturalWidth;
        canvas.height = img.naturalHeight;
        const ctx = canvas.getContext("2d");

        // Eerst tekenen
        ctx.drawImage(img, 0, 0);

        // ===== 3️⃣ Helderheid meten =====
        const imageData = ctx.getImageData(0, 0, canvas.width, canvas.height);
        const data = imageData.data;

        let totalBrightness = 0;
        const pixelCount = data.length / 4;

        for (let i = 0; i < data.length; i += 4) 
        {
            const brightness =
                0.299 * data[i] +
                0.587 * data[i + 1] +
                0.114 * data[i + 2];

            totalBrightness += brightness;
        }

        const averageBrightness = totalBrightness / pixelCount;
        console.log("Average brightness:", averageBrightness);

        // ===== 4️⃣ Dark mode toepassen als nodig =====
        // ===== Custom smooth recolor =====
        if (averageBrightness > 10) 
        {
            const darkColor  = [178, 172, 162]; // 87847d
            const lightColor = [81, 83, 90];    // 51535a

            for (let i = 0; i < data.length; i += 4) 
            {
                const r = data[i];
                const g = data[i + 1];
                const b = data[i + 2];

                // helderheid berekenen
                const brightness =
                    0.299 * r +
                    0.587 * g +
                    0.114 * b;

                const factor = brightness / 255; // 0 → zwart, 1 → wit

                data[i]     = darkColor[0]  + factor * (lightColor[0]  - darkColor[0]);
                data[i + 1] = darkColor[1]  + factor * (lightColor[1]  - darkColor[1]);
                data[i + 2] = darkColor[2]  + factor * (lightColor[2]  - darkColor[2]);
            }

            ctx.putImageData(imageData, 0, 0);
        }

        // ===== 5️⃣ Preview & opslaan =====
        const finalImage = canvas.toDataURL("image/png");

        imagePreview.src = finalImage;
        imagePreview.style.display = "block";
        placeholder.style.display = "none";

        uploadedImageData = finalImage;
    };

    img.src = objectURL;
}



const gridXInput = 16;
const gridYInput = 8;
let lastClickedIndicator = null;
function generateGridBtn(activeIndex = null) 
{
      document.querySelectorAll(".gridButton").forEach(b => b.remove());

    const cols = gridXInput;
    const rows = gridYInput;

    for (let i = 0; i < cols; i++) {
        for (let j = 0; j < rows; j++) {

            const btn = document.createElement("div");
            btn.className = "gridButton";
            btn.dataset.x = i;
            btn.dataset.y = j;

            const indicator = document.createElement("div");
            indicator.className = "gridButtonindIcator";
            indicator.dataset.x = i;
            indicator.dataset.y = j;

            // plaats vak exact in grid
            btn.style.left = `${(i / cols) * 100}%`;
            btn.style.top  = `${(j / rows) * 100}%`;

            btn.addEventListener("click", (e) => {
                if(kamerList.length <= 0) return;
                e.stopPropagation();

                selectedX = i;
                selectedY = j;
                isDirty = true;
                console.debug("t");
                if (lastClickedIndicator)
                    lastClickedIndicator.classList.remove("active");

                const activeKamerIndex = kamerList.findIndex(k =>
                    k[0].classList.contains("active")
                );

                if (activeKamerIndex !== -1) {
                    kamerList[activeKamerIndex][1] = [i, j];
                }

                indicator.classList.add("active");
                lastClickedIndicator = indicator;
            });

            btn.appendChild(indicator);
            dropZone.appendChild(btn);
        }
    }

    // actieve kamer herstellen
    if (activeIndex !== null && kamerList.length > activeIndex) {
        const coords = kamerList[activeIndex][1];
        if (coords) {
            const [x, y] = coords;
            const ind = [...document.querySelectorAll(".gridButtonindIcator")]
                .find(b => +b.dataset.x === x && +b.dataset.y === y);

            if (ind) {
                ind.classList.add("active");
                lastClickedIndicator = ind;
            }
        }
    }

    document.querySelectorAll(".axis-label").forEach(l => l.remove());

    // X-as labels (onder)
    for (let i = 0; i < cols; i++) {
        const label = document.createElement("div");
        label.style.color = "#b2aca2";
        label.className = "axis-label";
        label.textContent = i;

        const xPerc = (i + 0.5) / cols;
        label.style.left = `${xPerc * 100}%`;
        label.style.top = "100%";
        label.style.transform = "translate(-50%, 4px)";

        dropZone.appendChild(label);
    }

    // Y-as labels (links)
    for (let j = 0; j < rows; j++) {
        const label = document.createElement("div");
        label.style.color = "#b2aca2";
        label.className = "axis-label";
        label.textContent = j;

        const yPerc = (j + 0.5) / rows;
        label.style.left = "17px";
        label.style.top = `${yPerc * 100}%`;
        label.style.transform = "translate(-16px, -50%)";

        dropZone.appendChild(label);
    }

}

// oude labels opruimen


const kamerSettingsDiv1 = document.querySelector(".kamer-settings");
const kamerSettingsDiv2 = document.querySelector(".global-settings");
// Functie om settings tabel te genereren
function generateSettingsTable(settings, kamerSettingsDiv) {
    kamerSettingsDiv.innerHTML = "";

    const table = document.createElement("table");
    table.className = "settings-table";

    settings.forEach(setting => {
        const row = document.createElement("tr");

        // ===== LABEL =====
        const tdLabel = document.createElement("td");
        tdLabel.className = "settings-label";

        const labelWrapper = document.createElement("span");
        labelWrapper.textContent = setting.label;

        if (setting.help) {
            const tooltip = document.createElement("span");
            tooltip.className = "tooltip";
            tooltip.innerHTML = setting.help;

            labelWrapper.classList.add("has-tooltip");
            labelWrapper.appendChild(tooltip);
        }

        tdLabel.appendChild(labelWrapper);

        // ===== INPUT =====
        const tdInput = document.createElement("td");
        tdInput.className = "settings-input-container";

        let field;

        if (setting.type === "select") {
            field = document.createElement("select");

            setting.options.forEach(opt => {
                const option = document.createElement("option");
                option.value = opt.value;
                option.textContent = opt.label;
                option.selected = opt.value === setting.value;
                field.appendChild(option);
            });

        } else {
            field = document.createElement("input");
            field.type = setting.type || "text";
            field.value = setting.value ?? "";
        }

        field.className = "settings-input";
        field.dataset.key = setting.key;

        // ===== RANGE EXTRA'S =====
        if (setting.type === "range") {
            field.min = setting.min ?? 1;
            field.max = setting.max ?? 5;
            field.step = setting.step ?? 1;

            tdInput.style.display = "flex";
            tdInput.style.alignItems = "center";
            tdInput.style.gap = "8px";
            field.style.flexGrow = "1";
            field.style.minWidth = "200px";

            const valueLabel = document.createElement("span");
            valueLabel.style.color = '#b2aca2';
            valueLabel.textContent = field.value;

            field.addEventListener("input", () => {
                valueLabel.textContent = field.value;
            });

            tdInput.appendChild(valueLabel);
            tdInput.appendChild(field);

            field.value = setting.value;
            valueLabel.textContent = setting.value;

        } else {
            tdInput.appendChild(field);
        }

        // ===== ROW =====
        row.appendChild(tdLabel);
        row.appendChild(tdInput);
        table.appendChild(row);
    });

    kamerSettingsDiv.appendChild(table);

    

    // ===== INPUT LISTENERS + CHECKS =====
    kamerSettingsDiv.querySelectorAll(".settings-input").forEach(field => {
        field.addEventListener("input", (e) => {


            if (field.type === "text" && field.value.length > 50) {
    alert("Mag maximaal 50 tekens bevatten.");
    field.value = field.value.slice(0, 50); // knip af op 50 chars
    return;
}

            if (kamerSettingsDiv === kamerSettingsDiv1 && kamerList.length > index) {

                // ===== ANTWOORD LIMIT CHECK =====
                if (field.dataset.key === "antwoord") {
                    const antwoorden = field.value
                        .split(",")
                        .map(a => a.trim())
                        .filter(a => a !== "");

                    if (antwoorden.length > 5) {
                        alert("Maximaal 5 antwoorden toegestaan.");
                        antwoorden.pop();
                        field.value = antwoorden.join(", ");
                        return;
                    }
                }

                kamerList[index][2] = getSettingsData(kamerSettingsDiv);

                if (field.dataset.key === "naam-kamer") {
                    kamerList[index][0].childNodes[0].nodeValue = field.value;
                }

            }  else {

    // ===== START TIJD CHECK =====
    if (field.dataset.key === "start-tijd") {
        const value = field.value.trim();
            const intValue = parseInt(value, 10);
            console.log(intValue);
        // ✅ Leeg toestaan
        if (value === "") 
        {
            globalSettings = getSettingsData(kamerSettingsDiv);
            return;
        }
    
        if (!/^\d+$/.test(value)) 
        {
            alert("Start tijd moet een positief geheel getal zijn.");
            field.value = globalSettings["start-tijd"] || "";
            return;
        }

        if (intValue < 0) {
            alert("Start tijd mag niet negatief zijn.");
            field.value = "";
            return;
        }

        field.value = intValue;
    }

    globalSettings = getSettingsData(kamerSettingsDiv);
}
            isDirty = true;
        });
    });
}

function loadSettings(kamerSettingsDiv, data) 
{
    kamerSettingsDiv.querySelectorAll(".settings-input").forEach(field => {
        const key = field.dataset.key;
        if (!key || data[key] === undefined) return;

        if (field.tagName === "SELECT") {
            field.value = data[key];
       } else if (field.type === "range") {
            field.value = data[key];
            const valueLabel = field.parentElement.querySelector("span"); // pak de label
            if (valueLabel) valueLabel.textContent = data[key];
        } else {
            field.value = data[key];
        }
    });
}




// Voorbeeld aanroepen:
generateSettingsTable([
    {  key: "naam-kamer", label: "Naam kamer", type: "text", value: "Kamer " + index,help:"De naam van de kamer"},
    { key: "becon-ip", label: "Beacon ip", type:  "text", value: "" , help:"Het aderes van de becon"},
    { key: "antwoord",label: "Antwoord", type:  "text", value: "", help:"Welk antwoord er goed zijn <br> Gebruik een ',' om meerdere antwoorden in te vullen <br>" },
    { key: "open-compartment",label: "Open compartment",type: "select", value: 0, options: [
            { label: "Geen", value: 0 },
            { label: "Compartment 1", value: 1 },
            { label: "Compartment 2", value: 2 },
            { label: "Compartment 3", value: 3 }],
            help:"Welke ruimte er open gaat na het oplossen van de pussel"
    },
    { key: "special-acties",label: "Special acties",type: "select", value: 0, options: [
            { label: "Geen", value: 0 },
            { label: "Knip draat", value: 1 },
            { label: "Draai sluetel", value: 2 },
            { label: "Vinger print", value: 3 }],
            help:"hebt keuzen om spechale actie uit te moeten voeren "
    }
], kamerSettingsDiv1);


// Tip en na hoe lang in kamer
// 

generateSettingsTable([
    {
        key: "moeilijkheid",
        label: "Moeilijkheid",
        type: "range",
        value: 3,
        min: 1,
        max: 5,
        step: 1,
        help: "Hoe hoger de moeilijkheid, hoe strenger de straf bij foute antwoorden.<br><br>\
        <b>1</b>: -1 min, spel gaat door<br>\
        <b>2</b>: -5 min, spel gaat door<br>\
        <b>3</b>: -5 min, spel stopt bij 0<br>\
        <b>4</b>: -15 min, spel stopt bij 0<br>\
        <b>5</b>: resterende tijd /2, spel stopt bij 0"
        },
    { key: "start-tijd",label: "Start tijd", type:  "text", value: 60 , help:"Hoeveel tijd er is om het spel op te lossen"},
    { key: "audio",label: "Audio ", type: "select", value: 0, options: [
            { label: "Aan", value: 0 },
            { label: "Uit", value: 1 },],
            help:"Toggle sound effects en getik "  
        }
], kamerSettingsDiv2);


let isDirty = false;
window.addEventListener("beforeunload", (e) => 
{
    if(isDirty)
    {
        e.preventDefault();
        e.returnValue = "";
    }

});

function getSettingsData(container) 
{
    const data = {};

    container.querySelectorAll(".settings-input").forEach(field => 
    {
        const key = field.dataset.key;
        if (!key) return;

        if (field.type === "number" || field.type === "range") 
        {
            data[key] = Number(field.value);
        } else {
            data[key] = field.value;
        }
    });

    return data;
}

window.addEventListener("DOMContentLoaded", async () => 
{
    const data = await loadData();
});

async function loadData() 
{
    try 
    {
        const response = await fetch("/load");
        if (!response.ok) throw new Error("Fout bij laden van data");

        const data = await response.json(); // hier zit alles in: Recordings, kamerList, globalSettings, uploadedImageData

        // ✅ Recordings vullen
        Recordings.length = 0; // oude data wissen
        if (data.Recordings) 
        {
            data.Recordings.forEach(r => 
            {
                Recordings.push(r); // gewoon de array zoals hij is
            });
        }

        // ✅ kamerList vullen
        kamerList.length = 0; // oude data wissen
        if (data.kamerList) {
            data.kamerList.forEach(k => 
            {
                // in jouw JSON: k[0] = placeholder, k[1] = lege array, k[2] = object met data
                if (k[2]) 
                {
                    kamerList.push(
                    [null,k[1],
                    {
                        "naam-kamer": k[2]["naam-kamer"] || "",
                        "becon-ip": k[2]["becon-ip"] || "",
                        "antwoord": k[2]["antwoord"] || "",
                        "open-compartment": k[2]["open-compartment"] || 0,
                        "special-acties": k[2]["special-acties"] || 0
                    }]);
                }
            });
        }
        // ✅ uploadedImageData vullen
        uploadedImageData = data.uploadedImageData || null;

        // ✅ globalSettings vullen
        if (data.globalSettings) 
        {
            globalSettings = 
            {
                "moeilijkheid": data.globalSettings["moeilijkheid"] || 2,
                "start-tijd": data.globalSettings["start-tijd"] || 60,
                "audio": data.globalSettings["audio"] || 0
            };
        }

        console.debug("Recordings:", Recordings);
        console.debug("kamerList:", kamerList);
        console.debug("uploadedImageData:", uploadedImageData);
        console.debug("globalSettings:", globalSettings);

        drawRecordingsButtons();
        currentIndex = 0;
        drawChart();
        updateStats();
   
     
        if (uploadedImageData) showLoadedImage(uploadedImageData);

        loadSettings(kamerSettingsDiv2, globalSettings);
    } 
    catch (err) 
    {
        console.error(err);
    }
    initKamerList();
}

function showLoadedImage(base64) {
    if (!base64) return;

    imagePreview.src = base64;
    imagePreview.style.display = "block";
    placeholder.style.display = "none";
    ratioWarning.style.display = "none";

    uploadedImageData = base64; // state sync
}


        drawRecordingsButtons();
        currentIndex = 0;
        drawChart();
        updateStats();
async function saveToESP() 
{
        let resizedImage = null;
    if (myFile && myFile != null) 
        {
        // wacht tot image is verkleind
            resizedImage  = await new Promise((resolve) => 
            {
                downscaleImage(myFile, 5000, (resizedBase64) => {
                resolve(resizedBase64);
            });
        });
    }

    const payload = 
    {
        kamerList: kamerList.map(k => [null, k[1] || [], k[2]]),
        globalSettings: globalSettings,
        uploadedImageData: resizedImage 
       // uploadedImageData: myFile 
    };

    const res = await fetch("/save", 
    {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify(payload)
    });

    console.log(kamerList);
}


const btnSave = document.getElementById("btnSave");
const saveStatus = document.getElementById("saveStatus");

btnSave.addEventListener("click", async () => {
    await saveToESPWithStatus();
});

async function saveToESPWithStatus() {
    if (btnSave.disabled) return; // dubbelklik blokkeren

    btnSave.disabled = true;
    saveStatus.textContent = "⏳ Opslaan...";
    saveStatus.style.color = "#555";

    try {
        await saveToESP();

        saveStatus.textContent = "✅ Opgeslagen";
        saveStatus.style.color = "green";
        isDirty = false;

        // status na 2 sec weer leeg
        setTimeout(() => {
            saveStatus.textContent = "";
        }, 2000);

    } catch (err) {
        console.error(err);
        saveStatus.textContent = "❌ Opslaan mislukt";
        saveStatus.style.color = "red";
    } finally {
        btnSave.disabled = false;
    }
}

function downscaleImage(file, maxPixels, callback) {
    const img = new Image();
    const reader = new FileReader();

    reader.onload = (e) => { img.src = e.target.result; };

    img.onload = () => {
        let width = img.width;
        let height = img.height;

        const totalPixels = width * height;
        if (totalPixels > maxPixels) {
            const scale = Math.sqrt(maxPixels / totalPixels);
            width = Math.round(width * scale);
            height = Math.round(height * scale);
        }

        const canvas = document.createElement("canvas");
        canvas.width = width;
        canvas.height = height;
        const ctx = canvas.getContext("2d");
        ctx.drawImage(img, 0, 0, width, height);

        const resizedBase64 = canvas.toDataURL("image/png");
        callback(resizedBase64);
    };

    reader.readAsDataURL(file);
}
generateGridBtn();

function exportSettingsNoLib() {
    const settingsData = {
        kamerList,
        uploadedImageData,
        globalSettings
    };

    // JSON string
    const jsonStr = JSON.stringify(settingsData, null, 2);

    // Maak blob aan
    const blob = new Blob([jsonStr], { type: "application/json" });

    // Trigger download
    const a = document.createElement("a");
    a.href = URL.createObjectURL(blob);
    a.download = "settings.json"; // je kan .zip naam geven, maar het is geen echte ZIP
    a.click();
    URL.revokeObjectURL(a.href);
    console.log(settingsData);
}
const btnImport = document.getElementById("btnImport");

const importFileInputS = document.getElementById("importFile");
btnImport.addEventListener("click", () => {
    // Open File Explorer
    importFileInputS.click();
});

importFileInputS.addEventListener("change", (e) => {
    if (e.target.files.length > 0) {
        const file = e.target.files[0]; // dit is het gekozen bestand
        importSettingsNoLib(file);      // doorgeven aan je functie
    }
});

function importSettingsNoLib(file) 
{
     const reader = new FileReader();
    reader.onload = function(e) {
        try {
            const data = JSON.parse(e.target.result);




            // Kopieer data terug naar je variabelen
// 1. Maak de DOM leeg


    // 2. Reset alle kamer[0] naar null
    kamerList.forEach(k => k[0] = null);
    kamerList.length = 0; // oude data wissen
    if (data.kamerList) {
        data.kamerList.forEach(k => {
            const kamerData = k[2] || {
                "naam-kamer": "",
                "becon-ip": "",
                "antwoord": "",
                "open-compartment": 0,
                "special-acties": 0
            };
            kamerList.push([null, k[1] || [0,0], kamerData]);
                       
        });
    }
 console.log("Geïmporteerde data:", kamerList);
            uploadedImageData = data.uploadedImageData || null;
            globalSettings = data.globalSettings || { "moeilijkheid": 2, "start-tijd": 60, "audio": 0 };


        drawRecordingsButtons();
        currentIndex = 0;
        drawChart();
        updateStats();
            
            if (uploadedImageData) showLoadedImage(uploadedImageData);

     
            loadSettings(kamerSettingsDiv2, globalSettings);

            alert("Instellingen succesvol geïmporteerd!");
        } catch(err) {
            console.error("Fout bij importeren:", err);
            alert("Fout bij importeren: " + err.message);
        }

        initKamerList();
    };
    reader.readAsText(file);

}