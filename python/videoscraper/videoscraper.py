import yt_dlp

# Direct video URL
video_url = "https://player.vimeo.com/video/889905631?app_id=122963&h=1793c5542f&referrer=https%3A%2F%2Frise.articulate.com%2F"

# Download settings
download_options = {
    'format': 'bestvideo+bestaudio/best',  # Best video and audio quality
    'outtmpl': 'downloaded_video.%(ext)s',  # Save with a generic name
    'postprocessors': [{
        'key': 'FFmpegVideoConvertor',  # Use FFmpeg to convert
        'preferedformat': 'mp4',  # Convert to MP4
    }],
}

# Download the video
with yt_dlp.YoutubeDL(download_options) as ydl:
    ydl.download([video_url])  # Download from the provided URL
