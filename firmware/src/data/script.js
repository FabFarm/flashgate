document.addEventListener('DOMContentLoaded', function () {
    const form = document.getElementById('gate-settings');
    form.addEventListener('submit', function (event) {
        event.preventDefault();
        
        const highBeamPattern = document.getElementById('high-beam-pattern').value;
        const gateTimeout = document.getElementById('gate-timeout').value;
        // Initialize the MQTT client
        const clientId = 'ESP32_Gate_Frontend_' + new Date().getTime();
        const mqttClient = new Paho.MQTT.Client('your_mqtt_server', 9001, clientId);
        
        mqttClient.onConnectionLost = function (responseObject) {
            if (responseObject.errorCode !== 0) {
                console.log('MQTT connection lost: ' + responseObject.errorMessage);
            }
        };
        // Connect to the MQTT broker
        mqttClient.connect({
            userName: 'your_mqtt_user',
            password: 'your_mqtt_password',
            onSuccess: function () {
            // Prepare the JSON payload with the new settings
            const payload = JSON.stringify({
                highBeamPattern: highBeamPattern,
                gateTimeout: parseInt(gateTimeout),
            });
            
            // Send the settings to the ESP32 via MQTT
            const message = new Paho.MQTT.Message(payload);
            message.destinationName = 'homeassistant/gate/config';
            mqttClient.send(message);
  
            // Disconnect from the MQTT broker
            mqttClient.disconnect();
            },
            onFailure: function (error) {
                console.log('MQTT connection failed: ' + error.errorMessage);
            },
        });
    });
});
  