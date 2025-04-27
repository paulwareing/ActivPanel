const canvas = document.getElementById('scribbleCanvas');
const ctx = canvas.getContext('2d');

canvas.width = window.innerWidth;
canvas.height = window.innerHeight;

let drawing = false;
let currentContacts = {};

canvas.addEventListener('pointerdown', (event) => {
    drawing = true;
    const contactId = event.pointerId;
    currentContacts[contactId] = { x: event.clientX, y: event.clientY };
});

canvas.addEventListener('pointermove', (event) => {
    if (!drawing) return;

    const contactId = event.pointerId;
    if (currentContacts[contactId]) {
        const prevX = currentContacts[contactId].x;
        const prevY = currentContacts[contactId].y;

        currentContacts[contactId].x = event.clientX;
        currentContacts[contactId].y = event.clientY;

        ctx.lineWidth = 2;
        ctx.lineCap = 'round';
        ctx.strokeStyle = 'black';

        ctx.beginPath();
        ctx.moveTo(prevX, prevY);
        ctx.lineTo(event.clientX, event.clientY);
        ctx.stroke();
    }
});

canvas.addEventListener('pointerup', (event) => {
    delete currentContacts[event.pointerId];
    if (Object.keys(currentContacts).length === 0) {
        drawing = false;
    }
});

canvas.addEventListener('pointerleave', () => {
    drawing = false;
    currentContacts = {};
});