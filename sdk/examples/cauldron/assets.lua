-- Metadata

IconAssets = group{quality=9.95}
Icon = image{"images/icon.png"}


BootstrapGroup = group{ quality=8 }
LoadingBG = image{"images/TitleScreen_BG.png"}
PlayerLoadingBG = image{"images/UI/LoadingScreen.png"}
FloorBg = image{"images/Floor_BG_v2.png"}
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
PotionSleep = image{"images/Potions/Potion_Sleep_wtxt.png"}
PotionFlight = image{"images/Potions/Potion_Flight_wtxt.png"}
PotionLove = image{"images/Potions/Potion_Love_wtxt.png"}
PotionNeutral = image{"images/Potions/Potion_Neutral_wtxt.png"}
PotionPoison = image{"images/Potions/Potion_Poison_wtxt.png"}
PotionHaste = image{"images/Potions/Potion_Haste_wtxt.png"}
PotionVitality = image{"images/Potions/Potion_Vitality_wtxt.png"}
PotionLaughter = image{"images/Potions/Potion_Laughter_wtxt.png"}
CauldronUIBar00 = image{"images/UI/ProgressBar_00.png"}
CauldronUIBar30 = image{"images/UI/ProgressBar_30.png"}
CauldronUIBar70 = image{"images/UI/ProgressBar_70.png"}
CauldronUIBar100 = image{"images/UI/ProgressBar_100.png"}

Beep = sound{ "assets/beep.wav" }
