<template>
	<!-- Navigation -->
	<nav class="bg-blue-600 text-white p-4">
		<div class="container mx-auto flex justify-between items-center">
			<h1 class="text-2xl font-bold">Dashboard</h1>
			<div>
				<button class="bg-blue-500 hover:bg-blue-400 px-4 py-2 rounded">Logout</button>
			</div>
		</div>
	</nav>

	<div class="flex">
		<!-- Sidebar -->
		<aside class="w-64 bg-gray-800 text-white h-screen p-4">
			<h2 class="text-xl font-semibold mb-4">Navigation</h2>
			<ul>
				<li class="mb-2">
					<a href="#" class="block py-2 px-4 hover:bg-gray-700 rounded">Dashboard</a>
				</li>
				<li class="mb-2">
					<a href="#" class="block py-2 px-4 hover:bg-gray-700 rounded">Profile</a>
				</li>
				<li class="mb-2">
					<a href="#" class="block py-2 px-4 hover:bg-gray-700 rounded">Settings</a>
				</li>
				<li class="mb-2">
					<a href="#" class="block py-2 px-4 hover:bg-gray-700 rounded">Help</a>
				</li>
			</ul>
		</aside>

		<!-- Main Content -->
		<main class="flex-1 p-6">
			<h2 class="text-3xl font-semibold mb-6">Welcome to the Dashboard</h2>
			<div class="grid grid-cols-1 md:grid-cols-3 gap-6">
				<!-- Card 1 -->
				<div class="bg-white shadow rounded-lg p-4">
					<h3 class="text-lg font-semibold">Users</h3>
					<p class="text-gray-600 text-sm mt-2">100 active users</p>
				</div>
				<!-- Card 2 -->
				<div class="bg-white shadow rounded-lg p-4">
					<h3 class="text-lg font-semibold">Revenue</h3>
					<p class="text-gray-600 text-sm mt-2">$12,345</p>
				</div>
				<!-- Card 3 -->
				<div class="bg-white shadow rounded-lg p-4">
					<h3 class="text-lg font-semibold">Messages</h3>
					<p class="text-gray-600 text-sm mt-2">50 unread messages</p>
				</div>
			</div>
		</main>
	</div>
</template>>
