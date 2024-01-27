-- Metadata

IconAssets = group{quality=9.95}
Icon = image{"images/icon.png"}

-- Loading screen: Divided into two asset groups

BootstrapGroup = group{ quality=9.8 }
LoadingBg = image{ "images/loading.png" }

function frames(fmt, count)
    t = {}
    for i = 1, count do
        if (i % 2 == 0)
        then
           t[1+#t] = string.format(fmt, i)
        end
    end
    return t
end

CauldronGroup = group{quality=8}
Cauldron = image{ frames("images/Cauldron_Frames/Cauldron_1.%04d.png", 59)}
