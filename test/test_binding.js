const Notify = require("../lib/binding.js");

const instance = new Notify("TestApp", "com.test.app");
instance.show({ image: "", 
title: "Hello!", content: "Let's get started", actions: ["Start", "Stop"]}).then((t) => {
    console.log(t)
});
process.stdin.resume();
process.stdout.write("Enter to stop ");
process.stdin.once("data", function () {
  process.exit(0);
});