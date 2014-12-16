
boot = io.open("boot/boot.bin", "rb")
data = boot:read(448)
boot:close()

disk = io.open("hd.img", "r+b")
disk:seek("set", 62)
disk:write(data)
disk:close()