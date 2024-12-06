picture new_pic(int width , int height){
    picture pic;
    
    pic.width = width;
    pic.height = height;
    pic.pixels = (color*)malloc((pic.width*pic.height)*sizeof(color));
    color white = {255, 255, 255}; // Définition de la couleur blanche
    
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            set_pixel(pic, x, y, white);
            // Affecter la couleur blanche à chaque pixel de l'image
        }
    }

    return pic;
}

blzvkze
sev
vzsv
dvzesv