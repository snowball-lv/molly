
function write32(file, x)
    local b4=string.char(x%256) x=(x-x%256)/256
    local b3=string.char(x%256) x=(x-x%256)/256
    local b2=string.char(x%256) x=(x-x%256)/256
    local b1=string.char(x%256) x=(x-x%256)/256
    --file:write(b1,b2,b3,b4)
	file:write(b4,b3,b2,b1)
end

function write8(file, x)
    local b=string.char(x%256);
    file:write(b);
end

function fsize(file) 
	local current = file:seek();
	local size = file:seek("end");
	file:seek("set", current);
	return size;
end

function align8(file)
	local pos = file:seek();
	local new = (math.floor((pos + 7) / 8)) * 8;
	for i = 1, new - pos do
		write8(file, 0);
	end
end

local disk = io.open("disk.rfs", "w+b");

write32(disk, #arg);

align8(disk);

for i = 1, #arg do
	local file = io.open(arg[i], "rb");
	
	align8(disk);
	
	disk:write(arg[i]);
	write8(disk, 0);
	
	align8(disk);
	
	local size = fsize(file);
	write32(disk, size);
	
	align8(disk);
	
	disk:write(file:read("*all"));
	
	align8(disk);
	
	file:close();
end

disk:close()




















