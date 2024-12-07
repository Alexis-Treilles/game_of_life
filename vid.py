import os
import cv2

# Répertoire contenant les frames
frames_dir = "images"  # Chemin vers le répertoire contenant les frames
output_video = "game_of_life.mp4"  # Nom du fichier vidéo à générer
fps = 60  # Fréquence d'images par seconde

# Vérifier si le répertoire existe
if not os.path.exists(frames_dir):
    raise FileNotFoundError(f"Le répertoire {frames_dir} n'existe pas.")

# Lister les fichiers des frames et les trier par nom
frames = sorted([os.path.join(frames_dir, f) for f in os.listdir(frames_dir) if f.endswith(".ppm")])

# Vérifier s'il y a des frames
if not frames:
    raise FileNotFoundError(f"Aucune frame trouvée dans le répertoire : {frames_dir}")

# Charger la première frame pour récupérer les dimensions
first_frame = cv2.imread(frames[0])
if first_frame is None:
    raise ValueError(f"Impossible de lire la première frame : {frames[0]}")
height, width, _ = first_frame.shape

# Initialiser la vidéo avec OpenCV
fourcc = cv2.VideoWriter_fourcc(*'mp4v')  # Codec pour MP4
video = cv2.VideoWriter(output_video, fourcc, fps, (width, height))

# Ajouter chaque frame à la vidéo
for frame_file in frames:
    frame = cv2.imread(frame_file)
    if frame is None:
        print(f"Erreur lors de la lecture de la frame : {frame_file}")
        continue
    video.write(frame)

# Libérer les ressources
video.release()

print(f"Vidéo générée : {output_video}")
