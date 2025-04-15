<template>
	<header class="header flex justify-between items-center flex-wrap h-fit p-4 bg-primary-500 text-white">
		<section class="header-item-left flex sm:w-full items-center justify-between my-1">
			<div class="flex w-8 h-8 justify-center items-center" @click="toggleMenu">
				<i class="fas fa-bars"></i>
			</div>
			<h1 class="header-logo mx-auto text-xl font-normal ml-4">{{ title }}</h1>
			<div class="sm:flex !hidden w-8 h-8 justify-center items-center" @click="toggleNotify">
				<i class="fas fa-bell"></i>
			</div>
		</section>

		<div class="header-item-right flex-wrap !hidden"
			:class="[!isNotifyOpen ? ['flex', 'sm:hidden', 'justify-end'] : ['sm:flex', 'w-full', 'justify-between']]">
			<section class="flex items-center m-1">
				<i class="far fa-envelope pr-4"></i>
				<a class="font-semibold text-none" href="mailto:it@schulz-hygiene.de">it@schulz-hygiene.de</a>
			</section>

			<section class="flex items-center m-1">
				<div class="flex w-8 h-8 justify-center items-center">
					<i class="fas fa-search"></i>
				</div>
				<div class="flex w-8 h-8 justify-center items-center">
					<i class="far fa-envelope"></i>
				</div>
				<div class="flex w-8 h-8 justify-center items-center">
					<i class="far fa-bell"></i>
				</div>
			</section>
		</div>
	</header>
</template>

<script lang="ts" src="./header.component.ts"></script>
