if(process.platform === "win32") {
    const Notify = require('bindings')('notify-native.node').Notify

    class Notif {
        constructor(appName, aumi) {
            this._notify = new Notify(appName, aumi)
            this._groups = {}
        }

        show({ image, title, content, sound, actions, group }) {
            const soundEnabled = sound ? true : false
            return new Promise((resolve) => {
                const id = this._notify.show(image || '', title || '', content || '',  soundEnabled, actions || [], (status, action) => {
                    resolve({ status, action })
                })
                if(!this._groups[group]) {
                    this._groups[group] = []
                }
                this._groups[group].push(id)
            })
        }

        hide(group) {
            const arr = this._groups[group]
            arr && arr.forEach(id => {
                this._notify.hide(id)
            });
        }

        clear() {
            this._notify.clear()
        }
    }
    module.exports = Notif;
} else {
    class Notif {
        show() {
            return Promise.resolve({ status: 'failed' })
        }

        hide() {
        }
    }
    module.exports = Notif;
}
