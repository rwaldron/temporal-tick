var main = require("../");

main.tick(function() {
  console.log(1);
});


main.usleep(1, function() {
  console.log(2);
});
