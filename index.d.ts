export = Notify;

declare class Notify {
    constructor(appName: string, aumi: string)

    show(options: Notify.Options) : void
    hide(group: string) : void
}

declare namespace Notify {
    export interface Options {
        title: string
        content: string
        image?: string
        sound?: boolean
        actions?: Array<string>
        group?: string
    }
}
