const logDiv = document.getElementById('log');

function logEvent(event) {
  const { type, clientX, clientY, pointerType } = event;
  const pointerInfo = pointerType ? ` [pointerType: ${pointerType}]` : '';
  const logEntry = `${new Date().toLocaleTimeString()} - ${type} at (${Math.round(clientX)}, ${Math.round(clientY)})${pointerInfo}\n`;
  logDiv.textContent += logEntry;
  logDiv.scrollTop = logDiv.scrollHeight;
}

// Only log pointer events
const eventsToLog = ['pointerdown', 'pointermove', 'pointerup', 'pointercancel'];

// Attach event listeners for pointer events
eventsToLog.forEach(eventType => {
  document.addEventListener(eventType, event => {
    if (event.touches && event.touches.length > 0) {
      logEvent({ type: event.type, clientX: event.touches[0].clientX, clientY: event.touches[0].clientY });
    } else if (event.changedTouches && event.changedTouches.length > 0) {
      logEvent({ type: event.type, clientX: event.changedTouches[0].clientX, clientY: event.changedTouches[0].clientY });
    } else {
      logEvent(event);
    }
  });
});