export function generateColor(str) {
    if (!str || str.length === 0) return '#000000'; // Default black for empty strings

    const colors = [
        '#5C646A', // Dark Grayish Blue
        '#676F54', // Dark Grayish Olive
        '#7A6F6C', // Dark Grayish Red
        '#777C85', // Dark Cool Gray
        '#6D7D85', // Dark Grayish Cyan
        '#756C6E', // Dark Grayish Magenta
        '#6A6F57', // Dark Grayish Yellow
        '#5C656E', // Dark Grayish Teal
        '#746E6A', // Dark Grayish Brown
        '#807D7E', // Dark Warm Gray
        '#5C6166', // Dark Neutral Gray
        '#6B6867', // Dark Grayish Vermilion
        '#6D6965', // Dark Grayish Orange
        '#757170', // Dark Grayish Purple
        '#74746D'  // Dark Grayish Chartreuse
    ];
    return colors[str[0].codePointAt(0) % colors.length]
}