-- Metadata

IconAssets = group{quality=9.95}
Icon = image{"images/icon.png"}

-- Loading screen: Divided into two asset groups

BootstrapGroup = group{ quality=9.8 }
LoadingBg = image{"images/loading.png"}
FloorBg = image{"images/Floor_BG.png"}
Honey = image{"images/Ingredients/Honey.png", pinned=true}
FrogLegs = image{"images/Ingredients/FrogLegs.png", pinned=true}
Nightshade = image{"images/Ingredients/Nightshade.png", pinned=true}
Lavender = image{"images/Ingredients/Lavender.png", pinned=true}

DragonsBreath = image{"images/Ingredients/Dragon_sBreath.png", pinned=true}
Heart = image{"images/Ingredients/Heart.png", pinned=true}
GriffonFeather = image{"images/Ingredients/GriffonFeather.png", pinned=true}
HarpyBlood = image{"images/Ingredients/HarpyBlood.png", pinned=true}
DreamClouds = image{"images/Ingredients/DreamClouds.png", pinned=true}
CoffeeBeans = image{"images/Ingredients/CoffeeBeans.png", pinned=true}

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
