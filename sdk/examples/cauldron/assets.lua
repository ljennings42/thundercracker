-- Metadata

IconAssets = group{quality=9.95}
Icon = image{"images/icon.png"}

-- Loading screen: Divided into two asset groups

BootstrapGroup = group{ quality=9.8 }
LoadingBg = image{ "images/loading.png" }
PotionDrowsiness = image{"images/Potions/Potion_Drowsiness.png", pinned=true}
PotionFlight = image{"images/Potions/Potion_Flight.png", pinned=true}
PotionLove = image{"images/Potions/Potion_Love.png", pinned=true}
PotionNeutral = image{"images/Potions/Potion_Neutral.png", pinned=true}
PotionPoison = image{"images/Potions/Potion_Poison.png", pinned=true}
PotionSpeed = image{"images/Potions/Potion_Speed.png", pinned=true}
PotionVitality = image{"images/Potions/Potion_Vitality.png", pinned=true}

function frames(fmt, count)
    t = {}
    for i = 1, count do
        if (i % 4 == 0)
        then
           t[1+#t] = string.format(fmt, i)
        end
    end
    return t
end

CauldronGroup = group{quality=4}
Cauldron = image{ frames("images/Cauldron_Frames/Cauldron_1.%04d.png", 59)}
