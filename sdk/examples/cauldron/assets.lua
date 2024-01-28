-- Metadata

IconAssets = group{quality=9.95}
Icon = image{"images/icon.png"}


BootstrapGroup = group{ quality=5 }
LoadingBG = image{"images/TitleScreen_BG.png"}
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
PotionDrowsiness = image{"images/Potions/Potion_Drowsiness.png"}
PotionFlight = image{"images/Potions/Potion_Flight.png"}
PotionLove = image{"images/Potions/Potion_Love.png"}
PotionNeutral = image{"images/Potions/Potion_Neutral.png"}
PotionPoison = image{"images/Potions/Potion_Poison.png"}
PotionSpeed = image{"images/Potions/Potion_Speed.png"}
PotionVitality = image{"images/Potions/Potion_Vitality.png"}
