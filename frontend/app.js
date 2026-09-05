const authSection = document.getElementById("authSection");
const appSection = document.getElementById("appSection");
const authError = document.getElementById("authError");

const loginTab = document.getElementById("loginTab");
const registerTab = document.getElementById("registerTab");
const loginForm = document.getElementById("loginForm");
const registerForm = document.getElementById("registerForm");

const sessionInfo = document.getElementById("sessionInfo");
const logoutButton = document.getElementById("logoutButton");
const usersContainer = document.getElementById("users");
const pageInfo = document.getElementById("pageInfo");
const prevPageButton = document.getElementById("prevPage");
const nextPageButton = document.getElementById("nextPage");

const TOKEN_KEY = "pistache_token";
const USER_KEY = "pistache_user";

let currentPage = 1;
const pageLimit = 5;
let lastTotal = 0;

function getToken()
{
    return localStorage.getItem(TOKEN_KEY);
}

function getStoredUser()
{
    const raw = localStorage.getItem(USER_KEY);
    return raw ? JSON.parse(raw) : null;
}

function saveSession(token, user)
{
    localStorage.setItem(TOKEN_KEY, token);
    localStorage.setItem(USER_KEY, JSON.stringify(user));
}

function clearSession()
{
    localStorage.removeItem(TOKEN_KEY);
    localStorage.removeItem(USER_KEY);
}

function showAuthError(message)
{
    authError.textContent = message;
    authError.hidden = false;
}

function clearAuthError()
{
    authError.hidden = true;
}

function showApp()
{
    const user = getStoredUser();

    authSection.hidden = true;
    appSection.hidden = false;
    sessionInfo.textContent = user ? `Logged in as ${user.name} (${user.email})` : "Logged in";

    currentPage = 1;
    loadUsers();
}

function showAuth()
{
    authSection.hidden = false;
    appSection.hidden = true;
}

// --- tab switching ---

loginTab.addEventListener("click", () =>
{
    loginTab.classList.add("active");
    registerTab.classList.remove("active");
    loginForm.hidden = false;
    registerForm.hidden = true;
    clearAuthError();
});

registerTab.addEventListener("click", () =>
{
    registerTab.classList.add("active");
    loginTab.classList.remove("active");
    registerForm.hidden = false;
    loginForm.hidden = true;
    clearAuthError();
});

// --- auth ---

loginForm.addEventListener("submit", async (event) =>
{
    event.preventDefault();
    clearAuthError();

    const email = document.getElementById("loginEmail").value;
    const password = document.getElementById("loginPassword").value;

    try
    {
        const response = await fetch("/auth/login",
        {
            method: "POST",
            headers: { "Content-Type": "application/json" },
            body: JSON.stringify({ email, password })
        });

        const body = await response.json();

        if (!response.ok)
        {
            showAuthError(body.error || "Login failed");
            return;
        }

        saveSession(body.token, body.user);
        loginForm.reset();
        showApp();
    }
    catch (err)
    {
        showAuthError("Could not reach the server");
    }
});

registerForm.addEventListener("submit", async (event) =>
{
    event.preventDefault();
    clearAuthError();

    const name = document.getElementById("registerName").value;
    const email = document.getElementById("registerEmail").value;
    const password = document.getElementById("registerPassword").value;

    try
    {
        const response = await fetch("/auth/register",
        {
            method: "POST",
            headers: { "Content-Type": "application/json" },
            body: JSON.stringify({ name, email, password })
        });

        const body = await response.json();

        if (!response.ok)
        {
            showAuthError(body.error || "Registration failed");
            return;
        }

        saveSession(body.token, body.user);
        registerForm.reset();
        showApp();
    }
    catch (err)
    {
        showAuthError("Could not reach the server");
    }
});

logoutButton.addEventListener("click", () =>
{
    clearSession();
    showAuth();
});

// --- users list ---

async function loadUsers()
{
    const response = await fetch(`/users?page=${currentPage}&limit=${pageLimit}`);
    const body = await response.json();

    lastTotal = body.total;
    usersContainer.innerHTML = "";

    if (body.data.length === 0)
    {
        usersContainer.innerHTML = "<p>No users yet.</p>";
    }

    for (const user of body.data)
    {
        const element = document.createElement("div");
        element.className = "user";

        const info = document.createElement("div");
        info.className = "user-info";

        const name = document.createElement("strong");
        name.textContent = user.name;

        const email = document.createElement("span");
        email.textContent = user.email;

        info.appendChild(name);
        info.appendChild(email);

        const deleteButton = document.createElement("button");
        deleteButton.className = "delete-button";
        deleteButton.textContent = "Delete";
        deleteButton.addEventListener("click", () => deleteUser(user.id));

        element.appendChild(info);
        element.appendChild(deleteButton);

        usersContainer.appendChild(element);
    }

    const totalPages = Math.max(1, Math.ceil(lastTotal / pageLimit));
    pageInfo.textContent = `Page ${body.page} of ${totalPages}`;
    prevPageButton.disabled = body.page <= 1;
    nextPageButton.disabled = body.page >= totalPages;
}

prevPageButton.addEventListener("click", () =>
{
    if (currentPage > 1)
    {
        currentPage -= 1;
        loadUsers();
    }
});

nextPageButton.addEventListener("click", () =>
{
    currentPage += 1;
    loadUsers();
});

async function deleteUser(id)
{
    const response = await fetch(`/users/${id}`,
    {
        method: "DELETE",
        headers: { "Authorization": `Bearer ${getToken()}` }
    });

    if (response.status === 401)
    {
        // Token expired or invalid — send the user back to login.
        clearSession();
        showAuth();
        return;
    }

    if (!response.ok)
    {
        alert("Failed to delete user");
        return;
    }

    await loadUsers();
}

// --- boot ---

if (getToken())
{
    showApp();
}
else
{
    showAuth();
}
