
local boot = io.open("boot/boot.bin", "rb");
local data = boot:read(448);
boot:close();

local disk = assert(io.open("hd.img", "r+b"));
disk:seek("set", 62);
disk:write(data);
disk:close();