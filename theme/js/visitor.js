export function getVisitorInfo() {
    // Try to retrieve from localStorage
    let visitorInfo = JSON.parse(localStorage.getItem('visitorInfo'));

    // If not in localStorage, use the default and save to localStorage
    if (!visitorInfo) {
        visitorInfo = {
            name: "visitor",
            email: "visitor@visitor.com",
            commentIds: [1, 13, 17, 19],
        };

        // Save the default visitorInfo to localStorage
        localStorage.setItem('visitorInfo', JSON.stringify(visitorInfo));
    }

    return visitorInfo;
}