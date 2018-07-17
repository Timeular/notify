if(process.platform === "win32") {
    const Notify = require('../build/Release/notify-native').Notify

    class Notif {
        constructor(appName, aumi) {
            this._notify = new Notify(appName, aumi)
        }

        show({ image, title, content, actions }) {
            return new Promise((resolve) => {
                this._notify.show(image || '', title || '', content || '', actions || [], (status, action) => {
                    resolve({ status, action })
                })
            })
        }
    }
    module.exports = Notif;
} else {
    module.exports = Notif;
}
