#!/bin/bash

if [ $# -ne 3 ]; then
    echo "Usage: slices [world] [colors] [dir]"
    exit 2
fi

cat > $3/viewer.html <<EOF
<html>
<head>
<title>Minecraft MRI</title>
<style type="text/css">
#loading {
  background-color: black;
  position: fixed;
  padding: 5px;
  color: white;
  z-index: 1000;
  left: 5px;
  top: 5px;
}
</style>
<script type="text/javascript">
window.addEventListener('load',function() {
    var loading = document.getElementById('loading');
    var imgs = []; var n = 0; var pos = 0;
    for(var i = 0; i < 128; i++) {
      var img = document.createElement('img');
      img.style.position = "absolute";
      img.style.display = "none";
      img.style.left = "0px";
      img.style.top = "0px";
      var pad = ""+i;
      loading.innerHTML = (++n > 0) ? "Loading: "+n+" more" : "Viewing slice "+pos;
      while(pad.length < 3) pad = "0"+pad;
      img.onload = function() {
        loading.innerHTML = (--n > 0) ? "Loading: "+n+" more" : "Viewing slice "+pos;
      };
      img.src = "slice"+pad+".png?"+Math.random();
      document.body.appendChild(img);
      imgs.push(img);
    }
    function show(n) {
      imgs[n].style.display = "block";
      for(var i = 0; i < imgs.length; i++) {
        if(i != n) imgs[i].style.display = "none";
      }
      loading.innerHTML = "Viewing slice "+n;
    }
    var pos = 0; var size = 512;
    show(pos);
    window.addEventListener('keydown',function(e) {
        if(e.keyCode == 38) {
          e.preventDefault();
          if(pos < 127) show(++pos);
        } else if(e.keyCode == 40) {
          e.preventDefault();
          if(pos > 0) show(--pos);
        }
      },false);
  },false);
</script>
</head>
<body>
<div id="loading"></div>
</body>
</html>
EOF

./render "$1" "$2" "$3/slice.bmp" 0 0; sips -s format png "$3/slice.bmp" --out "$3/slice000.png"
./render "$1" "$2" "$3/slice.bmp" 1 1; sips -s format png "$3/slice.bmp" --out "$3/slice001.png"
./render "$1" "$2" "$3/slice.bmp" 2 2; sips -s format png "$3/slice.bmp" --out "$3/slice002.png"
./render "$1" "$2" "$3/slice.bmp" 3 3; sips -s format png "$3/slice.bmp" --out "$3/slice003.png"
./render "$1" "$2" "$3/slice.bmp" 4 4; sips -s format png "$3/slice.bmp" --out "$3/slice004.png"
./render "$1" "$2" "$3/slice.bmp" 5 5; sips -s format png "$3/slice.bmp" --out "$3/slice005.png"
./render "$1" "$2" "$3/slice.bmp" 6 6; sips -s format png "$3/slice.bmp" --out "$3/slice006.png"
./render "$1" "$2" "$3/slice.bmp" 7 7; sips -s format png "$3/slice.bmp" --out "$3/slice007.png"
./render "$1" "$2" "$3/slice.bmp" 8 8; sips -s format png "$3/slice.bmp" --out "$3/slice008.png"
./render "$1" "$2" "$3/slice.bmp" 9 9; sips -s format png "$3/slice.bmp" --out "$3/slice009.png"
./render "$1" "$2" "$3/slice.bmp" 10 10; sips -s format png "$3/slice.bmp" --out "$3/slice010.png"
./render "$1" "$2" "$3/slice.bmp" 11 11; sips -s format png "$3/slice.bmp" --out "$3/slice011.png"
./render "$1" "$2" "$3/slice.bmp" 12 12; sips -s format png "$3/slice.bmp" --out "$3/slice012.png"
./render "$1" "$2" "$3/slice.bmp" 13 13; sips -s format png "$3/slice.bmp" --out "$3/slice013.png"
./render "$1" "$2" "$3/slice.bmp" 14 14; sips -s format png "$3/slice.bmp" --out "$3/slice014.png"
./render "$1" "$2" "$3/slice.bmp" 15 15; sips -s format png "$3/slice.bmp" --out "$3/slice015.png"
./render "$1" "$2" "$3/slice.bmp" 16 16; sips -s format png "$3/slice.bmp" --out "$3/slice016.png"
./render "$1" "$2" "$3/slice.bmp" 17 17; sips -s format png "$3/slice.bmp" --out "$3/slice017.png"
./render "$1" "$2" "$3/slice.bmp" 18 18; sips -s format png "$3/slice.bmp" --out "$3/slice018.png"
./render "$1" "$2" "$3/slice.bmp" 19 19; sips -s format png "$3/slice.bmp" --out "$3/slice019.png"
./render "$1" "$2" "$3/slice.bmp" 20 20; sips -s format png "$3/slice.bmp" --out "$3/slice020.png"
./render "$1" "$2" "$3/slice.bmp" 21 21; sips -s format png "$3/slice.bmp" --out "$3/slice021.png"
./render "$1" "$2" "$3/slice.bmp" 22 22; sips -s format png "$3/slice.bmp" --out "$3/slice022.png"
./render "$1" "$2" "$3/slice.bmp" 23 23; sips -s format png "$3/slice.bmp" --out "$3/slice023.png"
./render "$1" "$2" "$3/slice.bmp" 24 24; sips -s format png "$3/slice.bmp" --out "$3/slice024.png"
./render "$1" "$2" "$3/slice.bmp" 25 25; sips -s format png "$3/slice.bmp" --out "$3/slice025.png"
./render "$1" "$2" "$3/slice.bmp" 26 26; sips -s format png "$3/slice.bmp" --out "$3/slice026.png"
./render "$1" "$2" "$3/slice.bmp" 27 27; sips -s format png "$3/slice.bmp" --out "$3/slice027.png"
./render "$1" "$2" "$3/slice.bmp" 28 28; sips -s format png "$3/slice.bmp" --out "$3/slice028.png"
./render "$1" "$2" "$3/slice.bmp" 29 29; sips -s format png "$3/slice.bmp" --out "$3/slice029.png"
./render "$1" "$2" "$3/slice.bmp" 30 30; sips -s format png "$3/slice.bmp" --out "$3/slice030.png"
./render "$1" "$2" "$3/slice.bmp" 31 31; sips -s format png "$3/slice.bmp" --out "$3/slice031.png"
./render "$1" "$2" "$3/slice.bmp" 32 32; sips -s format png "$3/slice.bmp" --out "$3/slice032.png"
./render "$1" "$2" "$3/slice.bmp" 33 33; sips -s format png "$3/slice.bmp" --out "$3/slice033.png"
./render "$1" "$2" "$3/slice.bmp" 34 34; sips -s format png "$3/slice.bmp" --out "$3/slice034.png"
./render "$1" "$2" "$3/slice.bmp" 35 35; sips -s format png "$3/slice.bmp" --out "$3/slice035.png"
./render "$1" "$2" "$3/slice.bmp" 36 36; sips -s format png "$3/slice.bmp" --out "$3/slice036.png"
./render "$1" "$2" "$3/slice.bmp" 37 37; sips -s format png "$3/slice.bmp" --out "$3/slice037.png"
./render "$1" "$2" "$3/slice.bmp" 38 38; sips -s format png "$3/slice.bmp" --out "$3/slice038.png"
./render "$1" "$2" "$3/slice.bmp" 39 39; sips -s format png "$3/slice.bmp" --out "$3/slice039.png"
./render "$1" "$2" "$3/slice.bmp" 40 40; sips -s format png "$3/slice.bmp" --out "$3/slice040.png"
./render "$1" "$2" "$3/slice.bmp" 41 41; sips -s format png "$3/slice.bmp" --out "$3/slice041.png"
./render "$1" "$2" "$3/slice.bmp" 42 42; sips -s format png "$3/slice.bmp" --out "$3/slice042.png"
./render "$1" "$2" "$3/slice.bmp" 43 43; sips -s format png "$3/slice.bmp" --out "$3/slice043.png"
./render "$1" "$2" "$3/slice.bmp" 44 44; sips -s format png "$3/slice.bmp" --out "$3/slice044.png"
./render "$1" "$2" "$3/slice.bmp" 45 45; sips -s format png "$3/slice.bmp" --out "$3/slice045.png"
./render "$1" "$2" "$3/slice.bmp" 46 46; sips -s format png "$3/slice.bmp" --out "$3/slice046.png"
./render "$1" "$2" "$3/slice.bmp" 47 47; sips -s format png "$3/slice.bmp" --out "$3/slice047.png"
./render "$1" "$2" "$3/slice.bmp" 48 48; sips -s format png "$3/slice.bmp" --out "$3/slice048.png"
./render "$1" "$2" "$3/slice.bmp" 49 49; sips -s format png "$3/slice.bmp" --out "$3/slice049.png"
./render "$1" "$2" "$3/slice.bmp" 50 50; sips -s format png "$3/slice.bmp" --out "$3/slice050.png"
./render "$1" "$2" "$3/slice.bmp" 51 51; sips -s format png "$3/slice.bmp" --out "$3/slice051.png"
./render "$1" "$2" "$3/slice.bmp" 52 52; sips -s format png "$3/slice.bmp" --out "$3/slice052.png"
./render "$1" "$2" "$3/slice.bmp" 53 53; sips -s format png "$3/slice.bmp" --out "$3/slice053.png"
./render "$1" "$2" "$3/slice.bmp" 54 54; sips -s format png "$3/slice.bmp" --out "$3/slice054.png"
./render "$1" "$2" "$3/slice.bmp" 55 55; sips -s format png "$3/slice.bmp" --out "$3/slice055.png"
./render "$1" "$2" "$3/slice.bmp" 56 56; sips -s format png "$3/slice.bmp" --out "$3/slice056.png"
./render "$1" "$2" "$3/slice.bmp" 57 57; sips -s format png "$3/slice.bmp" --out "$3/slice057.png"
./render "$1" "$2" "$3/slice.bmp" 58 58; sips -s format png "$3/slice.bmp" --out "$3/slice058.png"
./render "$1" "$2" "$3/slice.bmp" 59 59; sips -s format png "$3/slice.bmp" --out "$3/slice059.png"
./render "$1" "$2" "$3/slice.bmp" 60 60; sips -s format png "$3/slice.bmp" --out "$3/slice060.png"
./render "$1" "$2" "$3/slice.bmp" 61 61; sips -s format png "$3/slice.bmp" --out "$3/slice061.png"
./render "$1" "$2" "$3/slice.bmp" 62 62; sips -s format png "$3/slice.bmp" --out "$3/slice062.png"
./render "$1" "$2" "$3/slice.bmp" 63 63; sips -s format png "$3/slice.bmp" --out "$3/slice063.png"
./render "$1" "$2" "$3/slice.bmp" 64 64; sips -s format png "$3/slice.bmp" --out "$3/slice064.png"
./render "$1" "$2" "$3/slice.bmp" 65 65; sips -s format png "$3/slice.bmp" --out "$3/slice065.png"
./render "$1" "$2" "$3/slice.bmp" 66 66; sips -s format png "$3/slice.bmp" --out "$3/slice066.png"
./render "$1" "$2" "$3/slice.bmp" 67 67; sips -s format png "$3/slice.bmp" --out "$3/slice067.png"
./render "$1" "$2" "$3/slice.bmp" 68 68; sips -s format png "$3/slice.bmp" --out "$3/slice068.png"
./render "$1" "$2" "$3/slice.bmp" 69 69; sips -s format png "$3/slice.bmp" --out "$3/slice069.png"
./render "$1" "$2" "$3/slice.bmp" 70 70; sips -s format png "$3/slice.bmp" --out "$3/slice070.png"
./render "$1" "$2" "$3/slice.bmp" 71 71; sips -s format png "$3/slice.bmp" --out "$3/slice071.png"
./render "$1" "$2" "$3/slice.bmp" 72 72; sips -s format png "$3/slice.bmp" --out "$3/slice072.png"
./render "$1" "$2" "$3/slice.bmp" 73 73; sips -s format png "$3/slice.bmp" --out "$3/slice073.png"
./render "$1" "$2" "$3/slice.bmp" 74 74; sips -s format png "$3/slice.bmp" --out "$3/slice074.png"
./render "$1" "$2" "$3/slice.bmp" 75 75; sips -s format png "$3/slice.bmp" --out "$3/slice075.png"
./render "$1" "$2" "$3/slice.bmp" 76 76; sips -s format png "$3/slice.bmp" --out "$3/slice076.png"
./render "$1" "$2" "$3/slice.bmp" 77 77; sips -s format png "$3/slice.bmp" --out "$3/slice077.png"
./render "$1" "$2" "$3/slice.bmp" 78 78; sips -s format png "$3/slice.bmp" --out "$3/slice078.png"
./render "$1" "$2" "$3/slice.bmp" 79 79; sips -s format png "$3/slice.bmp" --out "$3/slice079.png"
./render "$1" "$2" "$3/slice.bmp" 80 80; sips -s format png "$3/slice.bmp" --out "$3/slice080.png"
./render "$1" "$2" "$3/slice.bmp" 81 81; sips -s format png "$3/slice.bmp" --out "$3/slice081.png"
./render "$1" "$2" "$3/slice.bmp" 82 82; sips -s format png "$3/slice.bmp" --out "$3/slice082.png"
./render "$1" "$2" "$3/slice.bmp" 83 83; sips -s format png "$3/slice.bmp" --out "$3/slice083.png"
./render "$1" "$2" "$3/slice.bmp" 84 84; sips -s format png "$3/slice.bmp" --out "$3/slice084.png"
./render "$1" "$2" "$3/slice.bmp" 85 85; sips -s format png "$3/slice.bmp" --out "$3/slice085.png"
./render "$1" "$2" "$3/slice.bmp" 86 86; sips -s format png "$3/slice.bmp" --out "$3/slice086.png"
./render "$1" "$2" "$3/slice.bmp" 87 87; sips -s format png "$3/slice.bmp" --out "$3/slice087.png"
./render "$1" "$2" "$3/slice.bmp" 88 88; sips -s format png "$3/slice.bmp" --out "$3/slice088.png"
./render "$1" "$2" "$3/slice.bmp" 89 89; sips -s format png "$3/slice.bmp" --out "$3/slice089.png"
./render "$1" "$2" "$3/slice.bmp" 90 90; sips -s format png "$3/slice.bmp" --out "$3/slice090.png"
./render "$1" "$2" "$3/slice.bmp" 91 91; sips -s format png "$3/slice.bmp" --out "$3/slice091.png"
./render "$1" "$2" "$3/slice.bmp" 92 92; sips -s format png "$3/slice.bmp" --out "$3/slice092.png"
./render "$1" "$2" "$3/slice.bmp" 93 93; sips -s format png "$3/slice.bmp" --out "$3/slice093.png"
./render "$1" "$2" "$3/slice.bmp" 94 94; sips -s format png "$3/slice.bmp" --out "$3/slice094.png"
./render "$1" "$2" "$3/slice.bmp" 95 95; sips -s format png "$3/slice.bmp" --out "$3/slice095.png"
./render "$1" "$2" "$3/slice.bmp" 96 96; sips -s format png "$3/slice.bmp" --out "$3/slice096.png"
./render "$1" "$2" "$3/slice.bmp" 97 97; sips -s format png "$3/slice.bmp" --out "$3/slice097.png"
./render "$1" "$2" "$3/slice.bmp" 98 98; sips -s format png "$3/slice.bmp" --out "$3/slice098.png"
./render "$1" "$2" "$3/slice.bmp" 99 99; sips -s format png "$3/slice.bmp" --out "$3/slice099.png"
./render "$1" "$2" "$3/slice.bmp" 100 100; sips -s format png "$3/slice.bmp" --out "$3/slice100.png"
./render "$1" "$2" "$3/slice.bmp" 101 101; sips -s format png "$3/slice.bmp" --out "$3/slice101.png"
./render "$1" "$2" "$3/slice.bmp" 102 102; sips -s format png "$3/slice.bmp" --out "$3/slice102.png"
./render "$1" "$2" "$3/slice.bmp" 103 103; sips -s format png "$3/slice.bmp" --out "$3/slice103.png"
./render "$1" "$2" "$3/slice.bmp" 104 104; sips -s format png "$3/slice.bmp" --out "$3/slice104.png"
./render "$1" "$2" "$3/slice.bmp" 105 105; sips -s format png "$3/slice.bmp" --out "$3/slice105.png"
./render "$1" "$2" "$3/slice.bmp" 106 106; sips -s format png "$3/slice.bmp" --out "$3/slice106.png"
./render "$1" "$2" "$3/slice.bmp" 107 107; sips -s format png "$3/slice.bmp" --out "$3/slice107.png"
./render "$1" "$2" "$3/slice.bmp" 108 108; sips -s format png "$3/slice.bmp" --out "$3/slice108.png"
./render "$1" "$2" "$3/slice.bmp" 109 109; sips -s format png "$3/slice.bmp" --out "$3/slice109.png"
./render "$1" "$2" "$3/slice.bmp" 110 110; sips -s format png "$3/slice.bmp" --out "$3/slice110.png"
./render "$1" "$2" "$3/slice.bmp" 111 111; sips -s format png "$3/slice.bmp" --out "$3/slice111.png"
./render "$1" "$2" "$3/slice.bmp" 112 112; sips -s format png "$3/slice.bmp" --out "$3/slice112.png"
./render "$1" "$2" "$3/slice.bmp" 113 113; sips -s format png "$3/slice.bmp" --out "$3/slice113.png"
./render "$1" "$2" "$3/slice.bmp" 114 114; sips -s format png "$3/slice.bmp" --out "$3/slice114.png"
./render "$1" "$2" "$3/slice.bmp" 115 115; sips -s format png "$3/slice.bmp" --out "$3/slice115.png"
./render "$1" "$2" "$3/slice.bmp" 116 116; sips -s format png "$3/slice.bmp" --out "$3/slice116.png"
./render "$1" "$2" "$3/slice.bmp" 117 117; sips -s format png "$3/slice.bmp" --out "$3/slice117.png"
./render "$1" "$2" "$3/slice.bmp" 118 118; sips -s format png "$3/slice.bmp" --out "$3/slice118.png"
./render "$1" "$2" "$3/slice.bmp" 119 119; sips -s format png "$3/slice.bmp" --out "$3/slice119.png"
./render "$1" "$2" "$3/slice.bmp" 120 120; sips -s format png "$3/slice.bmp" --out "$3/slice120.png"
./render "$1" "$2" "$3/slice.bmp" 121 121; sips -s format png "$3/slice.bmp" --out "$3/slice121.png"
./render "$1" "$2" "$3/slice.bmp" 122 122; sips -s format png "$3/slice.bmp" --out "$3/slice122.png"
./render "$1" "$2" "$3/slice.bmp" 123 123; sips -s format png "$3/slice.bmp" --out "$3/slice123.png"
./render "$1" "$2" "$3/slice.bmp" 124 124; sips -s format png "$3/slice.bmp" --out "$3/slice124.png"
./render "$1" "$2" "$3/slice.bmp" 125 125; sips -s format png "$3/slice.bmp" --out "$3/slice125.png"
./render "$1" "$2" "$3/slice.bmp" 126 126; sips -s format png "$3/slice.bmp" --out "$3/slice126.png"
./render "$1" "$2" "$3/slice.bmp" 127 127; sips -s format png "$3/slice.bmp" --out "$3/slice127.png"
rm "$3/slice.bmp"
